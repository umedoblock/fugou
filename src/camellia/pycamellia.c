#include "Python.h"
#include "structmember.h"

#define Camellia_MODULE

#include "camellia.h"

#define CM_KEY(obj) ((((CamelliaObject *)obj)->cm_key))

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    CAMELLIA_KEY cm_key;
} CamelliaObject;

static PyMemberDef Camellia_members[] = {
// #define offsetof(type, member) ( (int) & ((type*)0) -> member )
    {"key_size", T_UINT, offsetof(CamelliaObject, cm_key.keysize), 0,
        "Camellia Key Length"},
    {NULL}  // Sentinel
};

void dump(uchar *data, int length, int width)
{
    int i;
    for(i=0;i<length;i++){
        fprintf(stderr, "%02x ", data[i]);
        if((i+1)%width == 0)
            fprintf(stderr, "\n");
    }
    if(i%width)
        fprintf(stderr, "\n");
}

static PyObject *
Camellia_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CamelliaObject *self = NULL;

    self = (CamelliaObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        CM_KEY(self).keysize = 0;
    }

    return (PyObject *)self;
}

static void
Camellia_dealloc(CamelliaObject* self)
{
    camellia_destroy(&CM_KEY(self));
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
_encrypt(CamelliaObject *self, PyObject *args)
{
    Py_buffer text;
    char *m, *c;
    PyBytesObject *cipher;

    if (!PyArg_ParseTuple(args, "y*", &text))
        return NULL;
    m = (char *)text.buf;

    cipher = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, CM_BLOCKSIZE);
    if(!cipher){
        return NULL;
    }
    c = PyBytes_AsString((PyObject *)cipher);

    camellia_encrypt((uchar *)c, (uchar *)m, &CM_KEY(self));

    return (PyObject *)cipher;
}

static PyObject *
_decrypt(CamelliaObject *self, PyObject *args)
{
    Py_buffer cipher;
    char *c, *m;
    PyBytesObject *text;

    if (!PyArg_ParseTuple(args, "y*", &cipher))
        return NULL;

    c = (char *)cipher.buf;

    text = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, CM_BLOCKSIZE);
    if(!text){
        return NULL;
    }
    m = PyBytes_AsString((PyObject *)text);

    camellia_decrypt((uchar *)m, (uchar *)c, &CM_KEY(self));

    return (PyObject *)text;
}

typedef struct {
    unt text_size;
    unt cipher_size;
    unt snip_size;
    unt padding_size;
    unt block_size;
} _size_brother;

static void
_calc_size(_size_brother *sb, unt text_size, unt block_size)
{
    sb->text_size = text_size;
    sb->block_size = block_size;
    sb->snip_size = text_size % block_size;
    if(sb->snip_size == 0)
        sb->padding_size = block_size;
    else
        sb->padding_size = block_size - sb->snip_size;
    sb->cipher_size = block_size + text_size + sb->padding_size;
}

static void
encrypt_mode_cbc(
    uchar *c,
    uchar *iv,
    uchar *m,
    void *key,
    _size_brother *sb,
    void (*encrypt)(uchar *,uchar *,void *)
)
{
    int i;
    unt encrypted_size = 0, block_size = sb->block_size;
    uchar last_octet;

    memcpy(c, iv, block_size);
    while(encrypted_size < sb->cipher_size - block_size * 2){
        c += block_size;
        for(i=0;i<block_size;i++)
            c[i] = iv[i] ^ m[i];
        encrypt(c, c, key);
        iv = c;
        m += block_size;
        encrypted_size += block_size;
    }
    c += block_size;

    memcpy(c, m,  sb->snip_size);
    memset(c + sb->snip_size, 0x9f, sb->padding_size);
    last_octet = c[block_size - 1];
    last_octet = (last_octet & (~(block_size - 1))) | sb->snip_size;
    c[block_size - 1] = last_octet;

    for(i=0;i<sb->block_size;i++)
        c[i] ^= iv[i];
    encrypt(c, c, key);
}

static PyObject *
_encrypt_cbc(CamelliaObject *self, PyObject *args)
{
    Py_buffer text, iv_;
    unt text_size;
    char *m, *c, *iv;
    PyBytesObject *cipher;
    _size_brother sb;

    if (!PyArg_ParseTuple(args, "y*y*I", &iv_, &text, &text_size))
        return NULL;
    m = (char *)text.buf;
    iv = (char *)iv_.buf;
    _calc_size(&sb, text_size, CM_BLOCKSIZE);

    cipher = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, sb.cipher_size);
    if(!cipher){
        return NULL;
    }
    c = PyBytes_AsString((PyObject *)cipher);

    encrypt_mode_cbc((uchar *)c, (uchar *)iv, (uchar *)m, &CM_KEY(self),
                     &sb, (void *)camellia_encrypt);

    return (PyObject *)cipher;
}

static unt
decrypt_mode_cbc(
    uchar *m,
    uchar *c,
    void *key,
    _size_brother *sb,
    void (*decrypt)(uchar *,uchar *,void *)
)
{
    int i;
    uchar *d = m, *iv;
    unt text_size, decrypted_size = 0;
    unt block_size = sb->block_size, snip_size;

    iv = c;
    c += block_size;
    while(decrypted_size < sb->cipher_size - block_size){
        decrypt(d, c, key);
        for(i=0;i<block_size;i++)
            m[i] = iv[i] ^ d[i];
        iv = c;
        c += block_size;
        m += block_size;
        d = m;
        decrypted_size += block_size;
    }

    snip_size = m[-1] % block_size;
    text_size = sb->cipher_size - block_size * 2 + snip_size;
    _calc_size(sb, text_size, block_size);

    return text_size;
}

static PyObject *
_decrypt_cbc(CamelliaObject *self, PyObject *args)
{
    Py_buffer cipher;
    unt cipher_size, text_size;
    char *m, *c;
    PyBytesObject *text;
    _size_brother sb;

    if (!PyArg_ParseTuple(args, "y*I", &cipher, &cipher_size))
        return NULL;
    c = (char *)cipher.buf;

    text = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, cipher_size);
    if(!text){
        return NULL;
    }
    m = PyBytes_AsString((PyObject *)text);

    sb.cipher_size = cipher_size;
    sb.block_size = CM_BLOCKSIZE;
    text_size = decrypt_mode_cbc((uchar *)m, (uchar *)c, &CM_KEY(self),
                      &sb, (void *)camellia_decrypt);

    Py_SIZE(text) = text_size;
    m[text_size] = '\0';
    return (PyObject *)text;
}

static int
Camellia_init(CamelliaObject *self, PyObject *args, PyObject *kwds)
{
    Py_buffer key;
    uchar *uk;
    unt key_size = 0;

    if (!PyArg_ParseTuple(args, "y*I", &key, &key_size))
        return -1;
    uk = (uchar *)(key.buf);
    CM_KEY(self).keysize = key_size;
    camellia_keyset(&CM_KEY(self), uk, key_size);
    camellia_keygen(&CM_KEY(self));

    return 0;
}

static PyMethodDef Camellia_methods[] = {
    {"_encrypt", (PyCFunction )_encrypt, METH_VARARGS, "_encrypt()"},
    {"_decrypt", (PyCFunction )_decrypt, METH_VARARGS, "_decrypt()"},
    {"_encrypt_cbc", (PyCFunction )_encrypt_cbc,
        METH_VARARGS, "_encrypt_cbc()"},
    {"_decrypt_cbc", (PyCFunction )_decrypt_cbc,
        METH_VARARGS, "_decrypt_cbc()"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef Camellia_module = {
    PyModuleDef_HEAD_INIT,
    "_camellia", /* name of module */
    NULL, /* module documentation */
    -1,
    Camellia_methods
};

static PyTypeObject Camellia_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_camellia._Camellia",             /* tp_name */
    sizeof(CamelliaObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Camellia_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Camellia objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    Camellia_methods,             /* tp_methods */
    Camellia_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Camellia_init,      /* tp_init */
    0,                         /* tp_alloc */
    Camellia_new,                 /* tp_new */
    0,                         /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__camellia(void)
{
    PyObject *m;

    Camellia_Type.tp_new = Camellia_new;
    if (PyType_Ready(&Camellia_Type) < 0)
        return NULL;
    Py_INCREF(&Camellia_Type);

    m = PyModule_Create(&Camellia_module);
    if (m == NULL)
        return NULL;
    PyModule_AddObject(m, "_Camellia", (PyObject *)&Camellia_Type);

    PyModule_AddIntConstant(m, "BLOCK_SIZE", CM_BLOCKSIZE);

    return m;
}

PyMODINIT_FUNC
PyInit__camellia_pg(void)
{
    return PyInit__camellia();
}
