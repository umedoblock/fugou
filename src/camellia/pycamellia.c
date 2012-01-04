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

static PyMemberDef Camallia_members[] = {
// #define offsetof(type, member) ( (int) & ((type*)0) -> member )
    {"key_size", T_UINT, offsetof(CamelliaObject, cm_key.keysize), 0,
        "Camellia Key Length"},
    {NULL}  // Sentinel
};

static PyObject *
Camallia_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    CamelliaObject *self = NULL;
    self = (CamelliaObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        CM_KEY(self).keysize = 0;
    }

    return (PyObject *)self;
}

static void
Camallia_dealloc(CamelliaObject* self)
{
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
    unt cipher_size;
    unt snip_size;
    unt padding_size;
} size_brother;

static void
_calc_size(size_brother *sizes, unt text_size)
{
    sizes->snip_size = text_size % CM_BLOCKSIZE;
    if(sizes->snip_size == 0)
        sizes->padding_size = 0;
    else
        sizes->padding_size = CM_BLOCKSIZE - sizes->snip_size;
    sizes->cipher_size = CM_BLOCKSIZE + text_size + sizes->padding_size;
}

static void
encrypt_mode_cbc(
    uchar *c,
    uchar *iv,
    uchar *m,
    void *key,
    unt cipher_size,
    unt block_size,
    void (*encrypt)(uchar *,uchar *,void *)
)
{
    int i;

    memcpy(c, iv, block_size);
    c += block_size;
    cipher_size -= block_size;
    while(cipher_size){
        for(i=0;i<block_size;i++)
            c[i] = iv[i] ^ m[i];
        encrypt(c, m, key);
        iv = c;
        c += block_size;
        m += block_size;
        cipher_size -= block_size;
    }
}

static PyObject *
_encrypt_cbc(CamelliaObject *self, PyObject *args)
{
    Py_buffer text, iv_;
    unt text_size;
    char *m, *c, *iv;
    PyBytesObject *cipher;
    size_brother sb;

    if (!PyArg_ParseTuple(args, "y*y*I", &iv_, &text, &text_size))
        return NULL;
    m = (char *)text.buf;
    iv = (char *)iv_.buf;
    _calc_size(&sb, text_size);

    cipher = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, sb.cipher_size);
    if(!cipher){
        return NULL;
    }
    c = PyBytes_AsString((PyObject *)cipher);

    encrypt_mode_cbc((uchar *)c, (uchar *)iv, (uchar *)m, &CM_KEY(self),
                     sb.cipher_size, CM_BLOCKSIZE, camellia_encrypt);

    return (PyObject *)cipher;
}

static int
Camallia_init(CamelliaObject *self, PyObject *args, PyObject *kwds)
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

static PyMethodDef Camallia_methods[] = {
    {"_encrypt", (PyCFunction )_encrypt, METH_VARARGS, "_encrypt()"},
    {"_decrypt", (PyCFunction )_decrypt, METH_VARARGS, "_decrypt()"},
    {"_encrypt_cbc", (PyCFunction )_encrypt_cbc,
        METH_VARARGS, "_encrypt_cbc()"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef Camallia_module = {
    PyModuleDef_HEAD_INIT,
    "_camellia", /* name of module */
    NULL, /* module documentation */
    -1,
    Camallia_methods
};

static PyTypeObject Camallia_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_camellia.Camellia",             /* tp_name */
    sizeof(CamelliaObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Camallia_dealloc, /* tp_dealloc */
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
    Camallia_methods,             /* tp_methods */
    Camallia_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Camallia_init,      /* tp_init */
    0,                         /* tp_alloc */
    Camallia_new,                 /* tp_new */
    0,                         /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__camellia(void)
{
    PyObject *m;

    Camallia_Type.tp_new = Camallia_new;
    if (PyType_Ready(&Camallia_Type) < 0)
        return NULL;
    Py_INCREF(&Camallia_Type);

    m = PyModule_Create(&Camallia_module);
    if (m == NULL)
        return NULL;
    PyModule_AddObject(m, "_Camellia", (PyObject *)&Camallia_Type);

    PyModule_AddIntConstant(m, "BLOCK_SIZE", CM_BLOCKSIZE);

    return m;
}
