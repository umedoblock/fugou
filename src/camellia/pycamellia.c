#include "Python.h"
#include "structmember.h"

#define Camellia_MODULE

#include "camellia.h"

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
        self->cm_key.keysize = 0;
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
fprintf(stderr, "_encrypt()\n");
    if (!PyArg_ParseTuple(args, "y*", &text))
        return NULL;
fprintf(stderr, "_encrypt() end\n");
    Py_RETURN_NONE;
}

static PyObject *
_decrypt(CamelliaObject *self, PyObject *args)
{
    Py_buffer cipher;

fprintf(stderr, "_decrypt()\n");
    if (!PyArg_ParseTuple(args, "y*", &cipher))
        return NULL;
fprintf(stderr, "_decrypt() end\n");
    Py_RETURN_NONE;
}

static int
Camallia_init(CamelliaObject *self, PyObject *args, PyObject *kwds)
{
    Py_buffer key;
    uchar *uk;
    unt key_size = 0;

fprintf(stderr, "_init()\n");
    if (!PyArg_ParseTuple(args, "y*I", &key, &key_size))
        return -1;
    uk = (uchar *)(key.buf);
    self->cm_key.keysize = key_size;
    camellia_keyset(&(((CamelliaObject *)self)->cm_key), uk, key_size);
    camellia_keygen(&(((CamelliaObject *)self)->cm_key));
fprintf(stderr, "_init() end\n");
    return 0;
}

static PyMethodDef Camallia_methods[] = {
    {"_encrypt", (PyCFunction )_encrypt, METH_VARARGS, "_encrypt()"},
    {"_decrypt", (PyCFunction )_decrypt, METH_VARARGS, "_decrypt()"},
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
