#include "Python.h"
#include "structmember.h"

#define Bar_base_MODULE

char c_extension[] = "C_EXTENSION pybar.c";

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
} BarObject;

static PyObject *
Bar_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    BarObject *self = NULL;
fprintf(stderr, "Bar_new()\n");

    self = (BarObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
fprintf(stderr, "Py_TYPE(self) = %p\n", Py_TYPE(self));
fprintf(stderr, "sizeof(BarObject) = %d\n", sizeof(BarObject));
    }
fprintf(stderr, "\n");

    return (PyObject *)self;
}

#define _TP_BASE 1

static int
Bar_init(BarObject *self, PyObject *args, PyObject *kwds)
{
    int ret = 0;
    PyTypeObject *Bar_abstract = NULL, *Bar_base = NULL;
fprintf(stderr, "Bar_init()\n");

fprintf(stderr, "self = %p\n", self);
PyObject_Print((PyObject *)self, stderr, 0); fprintf(stderr, "\n");

Bar_base = Py_TYPE(self)->tp_base;
Py_INCREF(Bar_base);
fprintf(stderr, "Bar_base = %p\n", Bar_base);
PyObject_Print((PyObject *)Bar_base, stderr, 0); fprintf(stderr, "\n");

if (_TP_BASE){
Bar_abstract = Bar_base->tp_base;
Py_INCREF(Bar_abstract);
fprintf(stderr, "Bar_abstract = %p\n", Bar_abstract);
PyObject_Print((PyObject *)Bar_abstract, stderr, 0); fprintf(stderr, "\n");
}
else{
}

/*
endless loop
if (Bar_abstract->tp_init(self, args, kwds) < 0)
    return -1;
*/

// Py_DECREF(Bar_base);

fprintf(stderr, "Bar_init(ret=%d) done.\n", ret);
fprintf(stderr, "\n");
    return 0;
}

static void
Bar_dealloc(BarObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static struct PyModuleDef Bar_module = {
    PyModuleDef_HEAD_INIT,
    "_bar", /* name of module */
    NULL, /* module documentation */
    -1,
    NULL
};

static PyTypeObject Bar_base_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_bar.Bar_base",             /* tp_name */
    sizeof(BarObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Bar_dealloc, /* tp_dealloc */
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
#if _TP_BASE
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
#else
    Py_TPFLAGS_DEFAULT,
#endif
/*
#define Py_TPFLAGS_BASE_EXC_SUBCLASS    (1L<<30)
#define Py_TPFLAGS_TYPE_SUBCLASS        (1L<<31)
*/
                               /* tp_flags */
    "Bar_base class objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    0,                         /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Bar_init,      /* tp_init */
    0,                         /* tp_alloc */
    Bar_new,                 /* tp_new */
    0,                         /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__bar(void)
{
    PyObject *m;
    PyObject *import_bar = NULL, *Bar_abstract = NULL;

fprintf(stderr, "\n");
fprintf(stderr, "PyInit__bar(1)\n");

    import_bar = PyImport_ImportModule("bar");
    Bar_abstract = PyObject_GetAttrString(import_bar, "Bar_abstract");
fprintf(stderr, "Bar_abstract = %p\n", Bar_abstract);
PyObject_Print(Bar_abstract, stderr, 0);
fprintf(stderr, "\n");

if (_TP_BASE){
    Bar_base_Type.tp_base = (struct _typeobject *)Bar_abstract;
    Py_INCREF(Bar_base_Type.tp_base);
}
else{
if (1){
    Bar_base_Type.tp_base = &PyBaseObject_Type;
    Py_INCREF(Bar_base_Type.tp_base);
}
else{
    Bar_base_Type.tp_base = NULL;
}
}

    if (PyType_Ready(&Bar_base_Type) < 0)
        return NULL;
    Py_INCREF(&Bar_base_Type);

    m = PyModule_Create(&Bar_module);
fprintf(stderr, "Bar_base_Type.tp_base = %p\n", Bar_base_Type.tp_base);
PyObject_Print((PyObject *)Bar_base_Type.tp_base, stderr, 0);
fprintf(stderr, "\n");
fprintf(stderr, "Bar_base_Type.tp_mro = %p\n", Bar_base_Type.tp_mro);
PyObject_Print(Bar_base_Type.tp_mro, stderr, 0);
fprintf(stderr, "\n");

    if (m == NULL)
        return NULL;
    PyModule_AddObject(m, "Bar_base", (PyObject *)&Bar_base_Type);

fprintf(stderr, "&Bar_base_Type = %p\n", &Bar_base_Type);
PyObject_Print((PyObject *)&Bar_base_Type, stderr, 0);
fprintf(stderr, "\n");

    PyModule_AddStringConstant(m, "CONST", "C_EXTENSION Bar__init()");
fprintf(stderr, "return m=%p\n", m);
fprintf(stderr, "\n");

    return m;
}
