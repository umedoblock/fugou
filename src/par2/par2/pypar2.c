#include "Python.h"
#include "structmember.h"

#define Par2_MODULE

typedef unsigned short ushort;
typedef unsigned char uchar;

typedef struct {
    int poly;
    int bits;
    int w;
    int gf_max;
    int digits; /* need at view_matrix() for debug. */
    int redundancy;
    int octets;
    int vertical_size;
    ushort *gf;
    ushort *gfi;
    ushort *vander_matrix;
} par2_t;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    par2_t par2;
    char *mem;
} Par2Object;

static PyMemberDef Par2_members[] = {
// #define offsetof(type, member) ( (int) & ((type*)0) -> member )
    {"poly", T_INT, offsetof(Par2Object, par2.poly), 0, ""},
    {"bits", T_INT, offsetof(Par2Object, par2.bits), 0, ""},
    {"w", T_INT, offsetof(Par2Object, par2.w), 0, ""},
    {"gf_max", T_INT, offsetof(Par2Object, par2.gf_max), 0, ""},
    {"digits", T_INT, offsetof(Par2Object, par2.digits), 0, ""},
    {"redundancy", T_INT, offsetof(Par2Object, par2.redundancy), 0, ""},
    {"octets", T_INT, offsetof(Par2Object, par2.octets), 0, ""},
    {"vertical_size", T_INT, offsetof(Par2Object, par2.vertical_size), 0, ""},
    {NULL}  // Sentinel
};

static PyObject *
Par2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Par2Object *self = NULL;

fprintf(stderr, "Par2_new(type=%p, args=%p, kwds=%p)\n", type, args, kwds);
    self = (Par2Object *)type->tp_alloc(type, 0);
    if (self != NULL) {
fprintf(stderr, "self = %p\n", self);
fprintf(stderr, "type->tp_init = %p\n", type->tp_init);
fprintf(stderr, "self->mem = %p\n", self->mem);
    }
fprintf(stderr, "\n");

    return (PyObject *)self;
}

static int
Par2_init(Par2Object *self, PyObject *args, PyObject *kwds)
{
    int gf_and_gfi_size = 0, vander_matrix_size = 0;
    int allocate_size = 0;
    int bits = -1, redundancy = -1;
    PyObject *bits_obj = NULL, *redundancy_obj = NULL;

fprintf(stderr, "Par2_init(self=%p, args=%p, kwds=%p)\n", self, args, kwds);
    allocate_size = gf_and_gfi_size + vander_matrix_size + 100;
    self->mem = PyMem_Malloc(allocate_size);
fprintf(stderr, "PyMem_Malloc(allocate_size=%d) = %p\n", \
                                    allocate_size, self->mem);
    if (self->mem == NULL)
        return -1;

fprintf(stderr, "self->par2.poly=%d\n", self->par2.poly);
fprintf(stderr, "self->par2.bits=%d\n", self->par2.bits);
fprintf(stderr, "self->par2.w=%d\n", self->par2.w);
fprintf(stderr, "self->par2.gf_max = %d\n", self->par2.gf_max);
fprintf(stderr, "self->par2.digits = %d\n", self->par2.digits);
fprintf(stderr, "self->par2.redundancy = %d\n", self->par2.redundancy);
fprintf(stderr, "self->par2.octets = %d\n", self->par2.octets);
fprintf(stderr, "self->par2.vertical_size = %d\n", self->par2.vertical_size);

fprintf(stderr, "bits=%d\n", bits);
bits_obj = PyObject_GetAttrString((PyObject *)self, "bits");
fprintf(stderr, "bits_obj=\n");
PyObject_Print(bits_obj, stderr, 0);
fprintf(stderr, "\n");
Py_DECREF(bits_obj);

fprintf(stderr, "self->par2.redundancy=%d\n", self->par2.redundancy);
fprintf(stderr, "redundancy=%d\n", redundancy);
redundancy_obj = PyObject_GetAttrString((PyObject *)self, "redundancy");
fprintf(stderr, "redundancy_obj=\n");
PyObject_Print(redundancy_obj, stderr, 0);
fprintf(stderr, "\n");
Py_DECREF(redundancy_obj);

fprintf(stderr, "dir(self)=\n");
PyObject_Dir((PyObject *)self);
fprintf(stderr, "\n");

    return 0;
}

static void
Par2_dealloc(Par2Object* self)
{
fprintf(stderr, "Par2_dealloc(self=%p)\n", self);
    PyMem_Free(self->mem);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static void
Par2_free(Par2Object* self)
{
fprintf(stderr, "Par2_free(self=%p)\n", self);
    PyObject_Free(self);
}

void dump(void *ptr, int length, int width)
{
    int i;
    unsigned char *data = (unsigned char *)ptr;
    char fmt[80];

    sprintf(fmt, "0x%%0%dx ", sizeof(void *) * 2);
    fprintf(stderr, "dump() start = %p\n", ptr);

    fprintf(stderr, "%*s", 3 + sizeof(void *) * 2, "");
    for (i=0;i<width;i++){
        fprintf(stderr, "%02x ", (unsigned int )(data + i) & 0x0f);
        if ((i + 1) % 4 == 0) {
            fprintf(stderr, "| ");
        }
    }
    fprintf(stderr, "\n");

    for (i=0;i<length;i++) {
        if (i % width == 0) {
            fprintf(stderr, fmt, data + i);
        }
        fprintf(stderr, "%02x ", data[i]);
        if ((i + 1) % 4 == 0) {
            fprintf(stderr, "| ");
        }
        if ((i + 1) % width == 0) {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
}

static PyObject *
Py_dump(Par2Object *self, PyObject *args)
{
    PyObject *obj = NULL;
    int length;

    if (!PyArg_ParseTuple(args, "Oi", &obj, &length))
        return NULL;

    dump((void *)obj, length, 16);

    Py_RETURN_NONE;
}

static PyMethodDef Par2_methods[] = {
    {"dump", (PyCFunction )Py_dump, METH_VARARGS, "dump()"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef Par2_module = {
    PyModuleDef_HEAD_INIT,
    "_par2", /* name of module */
    NULL, /* module documentation */
    -1,
    Par2_methods
};

static PyTypeObject Par2_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_par2._Par2",             /* tp_name */
    sizeof(Par2Object),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Par2_dealloc, /* tp_dealloc */
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
    "Par2 objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    Par2_methods,             /* tp_methods */
    Par2_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Par2_init,      /* tp_init */
    0,               /* tp_alloc */
    Par2_new,                 /* tp_new */
    (freefunc)Par2_free,                /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__par2(void)
{
    PyObject *m;

    if (PyType_Ready(&Par2_Type) < 0)
        return NULL;

    m = PyModule_Create(&Par2_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Par2_Type);
    PyDict_SetItemString(Par2_Type.tp_dict, "C_EXTENSION", Py_True);

    PyModule_AddObject(m, "_Par2", (PyObject *)&Par2_Type);

fprintf(stderr, "Par2_Type.tp_dict = %p\n", Par2_Type.tp_dict);
PyObject_Print(Par2_Type.tp_dict, stderr, 0);
fprintf(stderr, "\n");

    return m;
}
