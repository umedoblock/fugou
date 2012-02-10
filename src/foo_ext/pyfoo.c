#include "Python.h"
#include "structmember.h"

#define Foo_MODULE

char c_extension[] = "C_EXTENSION pyfoo.c";

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    int num;
    char CONST[sizeof(c_extension)];
//  char ss[80];
} FooObject;

// static PyTypeObject Foo_Type;

/*
typedef struct PyMemberDef {
    char *name;
    int type;
    Py_ssize_t offset;
    int flags;
    char *doc;
} PyMemberDef;
*/

void PyTypeObject_dump(PyTypeObject *type, const char *msg)
{
if (msg)
    fprintf(stderr, "%s\n", msg);

fprintf(stderr, "type = %p\n", type);
fprintf(stderr, "tp_name = %s\n", type->tp_name);

fprintf(stderr, "tp_basicsize = %d\n", type->tp_basicsize);
fprintf(stderr, "tp_itemsize = %d\n", type->tp_itemsize);

    /* Methods to implement standard operations */

fprintf(stderr, "tp_dealloc = %p\n", type->tp_dealloc);
fprintf(stderr, "tp_print = %p\n", type->tp_print);
fprintf(stderr, "tp_getattr = %p\n", type->tp_getattr);
fprintf(stderr, "tp_setattr = %p\n", type->tp_setattr);
fprintf(stderr, "tp_reserved = %p\n", type->tp_reserved);
fprintf(stderr, "tp_repr = %p\n", type->tp_repr);

    /* Method suites for standard classes */
/*
    PyNumberMethods *tp_as_number;
    PySequenceMethods *tp_as_sequence;
    PyMappingMethods *tp_as_mapping;
*/

    /* More standard operations (here for binary compatibility) */

fprintf(stderr, "tp_hash = %p\n", type->tp_hash);
fprintf(stderr, "tp_call = %p\n", type->tp_call);
fprintf(stderr, "tp_str = %p\n", type->tp_str);
fprintf(stderr, "tp_getattro = %p\n", type->tp_getattro);
fprintf(stderr, "tp_setattro = %p\n", type->tp_setattro);

    /* Functions to access object as input/output buffer */
fprintf(stderr, "tp_as_buffer = %p\n", type->tp_as_buffer);

    /* Flags to define presence of optional/expanded features */
fprintf(stderr, "tp_flags = 0x%08x\n", type->tp_flags);
fprintf(stderr, "tp_doc = %s\n", type->tp_doc);

    /* call function for all accessible objects */
fprintf(stderr, "tp_traverse = %p\n", type->tp_traverse);

    /* delete references to contained objects */
fprintf(stderr, "tp_clear = %p\n", type->tp_clear);

    /* rich comparisons */
fprintf(stderr, "tp_richcompare = %p\n", type->tp_richcompare);

    /* weak reference enabler */
    // long tp_weaklistoffset;

    /* Iterators */
fprintf(stderr, "tp_iter = %p\n", type->tp_iter);
fprintf(stderr, "tp_iternext = %p\n", type->tp_iternext);

    /* Attribute descriptor and subclassing stuff */
fprintf(stderr, "tp_methods  = %p\n", type->tp_methods);
fprintf(stderr, "tp_members = %p\n", type->tp_members);

fprintf(stderr, "tp_getset  = %p\n", type->tp_getset);

fprintf(stderr, "tp_base  = %p\n", type->tp_base);
PyObject_Print(type->tp_base, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_dict = %p\n", type->tp_dict);
PyObject_Print(type->tp_dict, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_descr_get = %p\n", type->tp_descr_get);
PyObject_Print(type->tp_descr_get, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_descr_set = %p\n", type->tp_descr_set);
PyObject_Print(type->tp_descr_set, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_dictoffset = 0x%08x\n", type->tp_dictoffset);
fprintf(stderr, "tp_init = %p\n", type->tp_init);
PyObject_Print(type->tp_init, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_alloc = %p\n", type->tp_alloc);
fprintf(stderr, "tp_new = %p\n", type->tp_new);
fprintf(stderr, "tp_free = %p\n", type->tp_free);
fprintf(stderr, "tp_is_gc = %p\n", type->tp_is_gc);

fprintf(stderr, "tp_bases = %p\n", type->tp_bases);
PyObject_Print(type->tp_bases, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_mro = %p\n", type->tp_mro);
PyObject_Print(type->tp_mro, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_cache = %p\n", type->tp_cache);
PyObject_Print(type->tp_cache, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_subclasses = %p\n", type->tp_subclasses);
PyObject_Print(type->tp_subclasses, stderr, 0);fprintf(stderr, "\n");
fprintf(stderr, "tp_weaklist = %p\n", type->tp_weaklist);
PyObject_Print(type->tp_weaklist, stderr, 0);fprintf(stderr, "\n");

fprintf(stderr, "\n");
}

static PyMemberDef Foo_members[] = {
// #define offsetof(type, member) ( (int) & ((type*)0) -> member )
//  {"ss", T_STRING_INPLACE, offsetof(FooObject, ss), READONLY,
//      "Foo string"},
    {"num", T_INT, offsetof(FooObject, num), 0,
        "Foo number"},
    {"CONST", T_STRING_INPLACE, offsetof(FooObject, CONST), READONLY,
        "c extension"},
    {NULL}  // Sentinel
};

static PyObject *
Foo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    FooObject *self = NULL;
fprintf(stderr, "Foo_new()nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn\n");

    self = (FooObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
fprintf(stderr, "Py_TYPE(self) = %p<<<<<<<<<<<<<<<<<<<<<\n", Py_TYPE(self));
fprintf(stderr, "sizeof(FooObject) = %d\n", sizeof(FooObject));
    }

    return (PyObject *)self;
}

static PyTypeObject *
look_back_class(PyObject *obj, int depth)
{
    PyTypeObject *type, *type_ = (PyTypeObject *)(&PyBaseObject_Type);
    int i;

    fprintf(stderr, "look_back_class +++++++++++++++++++++++++++++++++++++\n");
    fprintf(stderr, "obj = %p\n", obj);
    PyObject_Print(obj, stderr, 0); fprintf(stderr, "\n\n");
    for(i=0,type=Py_TYPE(obj); type!=NULL && (i<depth||depth==0);
        i++,type=type->tp_base){
        fprintf(stderr, "type address = %p\n", type);
        fprintf(stderr, "tp_init = %p\n", type->tp_init);
        fprintf(stderr, "tp_base = %p\n", type->tp_base);
        fprintf(stderr, "tp_doc = %s\n", type->tp_doc);
        fprintf(stderr, "tp_name = %s\n", type->tp_name);
        PyObject_Print((PyObject *)type, stderr, 0); fprintf(stderr, "\n\n");
        type_ = type;
    }
    fprintf(stderr, "+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    return type_;
}

static int
Foo_init(FooObject *self, PyObject *args, PyObject *kwds)
{
    int ret = 0;
    PyTypeObject *super_type = &PySuper_Type;
    PyTypeObject *Foo_abstract = NULL, *Foo_base = NULL;
    PyObject *obj = NULL, *super_obj = NULL;
    PyObject *arg_tuple = NULL;

fprintf(stderr, "Foo_init()iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n");
fprintf(stderr, "Foo_base.__init__(Foo_init(5)=%p)>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", Foo_init);

fprintf(stderr, "self = %p\n", self);
PyObject_Print((PyObject *)self, stderr, 0); fprintf(stderr, "\n\n");

Foo_base = Py_TYPE(self)->tp_base;
Py_INCREF(Foo_base);
fprintf(stderr, "Foo_base = %p\n", Foo_base);
PyObject_Print((PyObject *)Foo_base, stderr, 0); fprintf(stderr, "\n\n");

Foo_abstract = Foo_base->tp_base;
Py_INCREF(Foo_abstract);
fprintf(stderr, "Foo_abstract = %p\n", Foo_abstract);
PyObject_Print((PyObject *)Foo_abstract, stderr, 0); fprintf(stderr, "\n\n");

PyTypeObject_dump(Py_TYPE(self), "Py_TYPE(self)");
PyTypeObject_dump(Foo_base, "PyTypeObject_dump(Foo_base)");
PyTypeObject_dump(Foo_abstract, "PyTypeObject_dump(Foo_abstract)");
/*
無限 loop
if (Foo_abstract->tp_init(self, args, kwds) < 0)
    return -1;
*/

// NULL, NULL でなんら問題なし。
super_obj = super_type->tp_new(super_type, NULL, NULL);
// arg_tuple = Py_BuildValue("(OO)", args, kwds);
arg_tuple = Py_BuildValue("(OO)", Foo_base, self);
fprintf(stderr, "super_type->tp_init(super_obj=%p, arg_tuple=%p, NULL)=%d\n", super_obj, arg_tuple, ret);
// 初期化
ret = super_type->tp_init(super_obj, arg_tuple, NULL);
fprintf(stderr, "super_obj = %p\n", super_obj);
PyObject_Print((PyObject *)super_obj, stderr, 0); fprintf(stderr, "\n\n");

// Py_DECREF(Foo_base);

// PyObject_CallFunction();

fprintf(stderr, "Foo_init(ret=%d) done.\n", ret);
    return 0;
}

static void
Foo_dealloc(FooObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
Foo_number(FooObject *self, PyObject *args)
{
    return Py_BuildValue("i", self->num);
}

static PyObject *
Foo_string(FooObject *self, PyObject *args)
{
//  return Py_BuildValue("s", self->ss);
    return Py_BuildValue("s", "string.");
}

static PyObject *
Foo_absmethod(FooObject *self, PyObject *args)
{
    return Py_BuildValue("s", "abstractmethod");
}

static PyMethodDef Foo_methods[] = {
    {"number", (PyCFunction )Foo_number, METH_NOARGS, "number()"},
    {"string", (PyCFunction )Foo_string, METH_NOARGS, "string()"},
    {"absmethod", (PyCFunction )Foo_absmethod,
        METH_NOARGS, "absmethod()"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef Foo_module = {
    PyModuleDef_HEAD_INIT,
    "_foo", /* name of module */
    NULL, /* module documentation */
    -1,
    Foo_methods
};

static PyTypeObject Foo_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_foo.Foo_base",             /* tp_name */
    sizeof(FooObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Foo_dealloc, /* tp_dealloc */
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
                               /* tp_flags */
    "Foo_base class objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    Foo_methods,             /* tp_methods */
    Foo_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Foo_init,      /* tp_init */
    0,                         /* tp_alloc */
    Foo_new,                 /* tp_new */
    0,                         /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__foo(void)
{
    PyObject *m;
    PyObject *import_foo = NULL, *Foo_abstract = NULL;

fprintf(stderr, "PyInit__foo(--------------------------------------------=)\n");

    import_foo = PyImport_ImportModule("foo");
    Foo_abstract = PyObject_GetAttrString(import_foo, "Foo_abstract");
fprintf(stderr, "\n");
fprintf(stderr, "Foo_abstract = %p\n", Foo_abstract);
PyObject_Print(Foo_abstract, stderr, 0);
fprintf(stderr, "\n");
    Foo_Type.tp_new = Foo_new;

    Foo_Type.tp_base = &PyBaseObject_Type;
    Foo_Type.tp_base = (struct _typeobject *)Foo_abstract;
    Py_INCREF(Foo_Type.tp_base);
    if (PyType_Ready(&Foo_Type) < 0)
        return NULL;
    Py_INCREF(&Foo_Type);
/*
fprintf(stderr, "((PyTypeObject *)Foo_abstract)->tp_init = %p\n", ((PyTypeObject *)Foo_abstract)->tp_init);
// PyBytes_Type;
fprintf(stderr, "\n");
fprintf(stderr, "&Foo_Type = %p<<<<<<<<<<<<<<<<<<<\n", &Foo_Type);
fprintf(stderr, "Foo_Type.tp_init = %p<<<<<<<<<<<\n", Foo_Type.tp_init);
PyObject_Print((PyObject *)&Foo_Type, stderr, 0);
fprintf(stderr, "\n");
*/
    m = PyModule_Create(&Foo_module);
fprintf(stderr, "Foo_Type.tp_base = %p\n", Foo_Type.tp_base);
PyObject_Print(Foo_Type.tp_base, stderr, 0);
fprintf(stderr, "\n");
fprintf(stderr, "Foo_Type.tp_mro = %p\n", Foo_Type.tp_mro);
PyObject_Print(Foo_Type.tp_mro, stderr, 0);
fprintf(stderr, "\n");

    if (m == NULL)
        return NULL;
    PyModule_AddObject(m, "Foo_base", (PyObject *)&Foo_Type);

fprintf(stderr, "&Foo_Type = %p\n", &Foo_Type);
PyObject_Print(&Foo_Type, stderr, 0);
fprintf(stderr, "\n");

    PyModule_AddStringConstant(m, "CONST", "C_EXTENSION Foo__init()");
fprintf(stderr, "return m=%p\n", m);
fprintf(stderr, "Foo_Type.tp_base = %p\n", Foo_Type.tp_base);
PyObject_Print(Foo_Type.tp_base, stderr, 0);
fprintf(stderr, "\n");
fprintf(stderr, "\n");

    return m;
}
