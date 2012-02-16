#include "Python.h"
#include "structmember.h"

#include "libpar2.h"

#define Par2_MODULE

static PyTypeObject PyPar2Type;

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    par2_t par2;
} PyPar2Object;

#define PyPar2_Check(op) PyObject_TypeCheck(op, &PyPar2Type)
#define PyPar2_CheckExact(op) (Py_TYPE(op) == &PyPar2Type)

static PyMemberDef Par2_members[] = {
// #define offsetof(type, member) ( (int) & ((type*)0) -> member )
    {"poly", T_INT, offsetof(PyPar2Object, par2.poly), 0, ""},
    {"bits", T_INT, offsetof(PyPar2Object, par2.bits), 0, ""},
    {"w", T_INT, offsetof(PyPar2Object, par2.w), 0, ""},
    {"gf_max", T_INT, offsetof(PyPar2Object, par2.gf_max), 0, ""},
    {"digits", T_INT, offsetof(PyPar2Object, par2.digits), 0, ""},
    {"redundancy", T_INT, offsetof(PyPar2Object, par2.redundancy), 0, ""},
    {"octets", T_INT, offsetof(PyPar2Object, par2.octets), 0, ""},
    {"vertical_size", T_INT, offsetof(PyPar2Object, par2.vertical_size), 0, ""},
    {"horizontal_size", T_INT, \
        offsetof(PyPar2Object, par2.horizontal_size), 0, ""},
    {NULL}  // Sentinel
};

static PyObject *
Par2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyPar2Object *self = NULL;

/*
fprintf(stderr, "Par2_new(type=%p, args=%p, kwds=%p)\n", type, args, kwds);
*/
    self = (PyPar2Object *)type->tp_alloc(type, 0);
    if (self != NULL) {
/*
fprintf(stderr, "self = %p\n", self);
fprintf(stderr, "type->tp_init = %p\n", type->tp_init);
fprintf(stderr, "self->mem = %p\n", self->mem);
*/
    }
/*
fprintf(stderr, "\n");
*/

    return (PyObject *)self;
}

static int
Par2_init(PyPar2Object *self, PyObject *args, PyObject *kwds)
{
    par2_t *p2 = &self->par2;
    int ret;

/*
fprintf(stderr, "Par2_init(self=%p, args=%p, kwds=%p)\n", self, args, kwds);
*/
    ret = par2_init_structure(p2, p2->bits, p2->redundancy);
    if (ret < 0){
        par2_view_structure(p2);
        return -1;
    }

    if (par2_allocate_memory(p2) < 0) {
        par2_view_structure(p2);
        return -1;
    }
    /*
    par2_view_structure(p2);
    */

    return 0;
}

static void
Par2_dealloc(PyPar2Object* self)
{
    par2_t *p2 = &self->par2;
/*
fprintf(stderr, "Par2_dealloc(self=%p)\n", self);
*/
    par2_free_memory(p2);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static void
Par2_free(PyPar2Object* self)
{
/*
fprintf(stderr, "Par2_free(self=%p)\n", self);
*/
    PyObject_Free(self);
}

static PyObject *
Par2__add(PyPar2Object *self, PyObject *args)
{
    ushort a, b, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &b))
        return NULL;

    c = par2_add(a, b);

    return Py_BuildValue("H", c);
}

static PyObject *
Par2__mul(PyPar2Object *self, PyObject *args)
{
    ushort a, b, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &b))
        return NULL;

    c = par2_mul(&self->par2, a, b);

    return Py_BuildValue("H", c);
}

static PyObject *
Par2__div(PyPar2Object *self, PyObject *args)
{
    ushort a, b, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &b))
        return NULL;

    c = par2_div(&self->par2, a, b);

    return Py_BuildValue("H", c);
}
/*
*/

static PyObject *
Par2__pow(PyPar2Object *self, PyObject *args)
{
    ushort a, x, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &x))
        return NULL;

    c = par2_pow(&self->par2, a, x);

    return Py_BuildValue("H", c);
}

static PyObject *
Par2__make_gf_and_gfi(PyPar2Object *self)
{
/*
fprintf(stderr, "Par2__make_gf_and_gfi(self=%p)\n", self);
*/
    par2_make_gf_and_gfi(&self->par2);

    Py_RETURN_NONE;
}

static PyObject *
Par2__get_vandermonde_matrix(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    ushort *vm;
    PyBytesObject *vandermonde_matrix = NULL;

/*
fprintf(stderr, "Par2__get_vandermonde_matrix(self=%p)\n", self);
*/
    vandermonde_matrix = \
        (PyBytesObject *)PyBytes_FromStringAndSize(NULL, p2->matrix_size);
    if (vandermonde_matrix == NULL)
        return NULL;
    vm = (ushort *)PyBytes_AS_STRING(vandermonde_matrix);

    memcpy(vm, p2->vandermonde_matrix, p2->matrix_size);

    return (PyObject *)vandermonde_matrix;
}

static PyObject *
Par2__make_vandermonde_matrix(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;

/*
fprintf(stderr, "Par2__make_vandermonde_matrix(self=%p)\n", self);
*/
    par2_make_vandermonde_matrix(p2);

    Py_RETURN_NONE;
}

static PyBytesObject *
Par2__make_square_matrix(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    int redundancy = p2->redundancy;
    int matrix_size = 0;
    PyBytesObject *matrix = NULL;

    matrix_size = sizeof(ushort) * redundancy * redundancy;
    matrix = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, matrix_size);

    return matrix;
}

static PyObject *
Par2__make_e_matrix(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    int redundancy = p2->redundancy;
    int i, matrix_size = 0;
    PyBytesObject *matrix = NULL;
    ushort *mt;

    matrix = Par2__make_square_matrix(self);
    if (matrix == NULL)
        return NULL;
    mt = (ushort *)PyBytes_AS_STRING(matrix);
    matrix_size = PyBytes_GET_SIZE(matrix);
    memset(mt, '\0', matrix_size);

    for(i=0;i<redundancy;i++)
        mt[i * redundancy + i] = 1;

    return (PyObject *)matrix;
}

static PyObject *
Par2__encode(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    PyObject *parity_slots_obj, *data_slots_obj, *slot_obj;
    int symbol_num;
    int allocate_size, len_slots, len_data_slots, len_parity_slots;
    uchar **slots, **data_slots, **parity_slots;
    int i;

    if (!PyArg_ParseTuple(args, "OOi", \
                          &parity_slots_obj, &data_slots_obj, &symbol_num))
        return NULL;

    allocate_size = sizeof(uchar *) * p2->redundancy * 2;
    slots = (uchar **)PyMem_Malloc(allocate_size);
/*
fprintf(stderr, "PyMem_Malloc(allocate_size=%d), redundancy=%d\n",
                 allocate_size, redundancy);
*/
    if (slots == NULL)
        return NULL;
    /*
    memset(slots, '\0', allocate_size);
    */
    data_slots = slots;
    parity_slots = slots + p2->redundancy;
    len_data_slots = PySequence_Length(data_slots_obj);
    len_parity_slots = PySequence_Length(parity_slots_obj);
    len_slots = len_data_slots + len_parity_slots;
    if (len_slots < p2->redundancy * 2) {
        /* to avoid noisy compiler */
    }
/*
fprintf(stderr, "len_slots = %d, len_data_slots = %d, len_parity_slots = %d\n",
                 len_slots, len_data_slots, len_parity_slots);
*/

    for (i=0;i<len_data_slots;i++) {
        slot_obj = PySequence_GetItem(data_slots_obj, i);
        data_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
/*
fprintf(stderr, "data_slots_obj[%d] =\n", i);
PyObject_Print(slot_obj, stderr, 0); fprintf(stderr, "\n");
*/
    }
/*
fprintf(stderr, "\n");
*/

    for (i=0;i<len_parity_slots;i++) {
        slot_obj = PySequence_GetItem(parity_slots_obj, i);
        parity_slots[i] = (uchar *)PyByteArray_AS_STRING(slot_obj);
/*
fprintf(stderr, "parity_slots_obj[%d] =\n", i);
PyObject_Print(slot_obj, stderr, 0); fprintf(stderr, "\n");
*/
    }
/*
fprintf(stderr, "\n");
*/

    par2_encode(p2, parity_slots, data_slots, symbol_num);

/*
fprintf(stderr, "after encode() ======================================\n");
*/
    for (i=0;i<len_data_slots;i++) {
        slot_obj = PySequence_GetItem(data_slots_obj, i);
        data_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
/*
fprintf(stderr, "data_slots_obj[%d] =\n", i);
PyObject_Print(slot_obj, stderr, 0); fprintf(stderr, "\n");
*/
    }
/*
fprintf(stderr, "\n");
*/

    for (i=0;i<len_parity_slots;i++) {
        slot_obj = PySequence_GetItem(parity_slots_obj, i);
        parity_slots[i] = (uchar *)PyByteArray_AS_STRING(slot_obj);
/*
fprintf(stderr, "parity_slots_obj[%d] =\n", i);
PyObject_Print(slot_obj, stderr, 0); fprintf(stderr, "\n");
*/
    }
/*
fprintf(stderr, "\n");
*/

    PyMem_Free(slots);

    Py_RETURN_NONE;
}

static PyObject *
Par2__solve_inverse_matrix(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    int ret;
    PyBytesObject *inverse_matrix = NULL;
    Py_buffer matrix;
    ushort *mt, *im;

    if (!PyArg_ParseTuple(args, "y*", &matrix))
        return NULL;
    mt = (ushort *)matrix.buf;

    inverse_matrix = (PyBytesObject *)Par2__make_e_matrix(self);
    if (inverse_matrix == NULL)
        return NULL;
    im = (ushort *)PyBytes_AS_STRING(inverse_matrix);

    ret = par2_solve_inverse_matrix(p2, im, mt);
    if (ret < 0){
        /* Par2RankError */
fprintf(stderr, "Par2RankError\n");
        return NULL;
    }

    return (PyObject *)inverse_matrix;
}

static PyObject *
Par2__mul_matrixes(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    PyBytesObject *answer = NULL, *a, *b;
    ushort *ans, *a_mt, *b_mt;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

    answer = Par2__make_square_matrix(self);
    if (answer == NULL)
        return NULL;

    ans = (ushort *)PyBytes_AS_STRING(answer);
    a_mt = (ushort *)PyBytes_AS_STRING(a);
    b_mt = (ushort *)PyBytes_AS_STRING(b);

    par2_mul_matrixes(p2, ans, a_mt, b_mt);

    return (PyObject *)answer;
}

static PyObject *
Py_dump(PyPar2Object *self, PyObject *args)
{
    PyObject *obj = NULL;
    int length;

    if (!PyArg_ParseTuple(args, "Oi", &obj, &length))
        return NULL;

    dump((void *)obj, length, 16);

    Py_RETURN_NONE;
}

static PyMethodDef Par2_methods[] = {
    {"_encode", (PyCFunction )Par2__encode, \
        METH_VARARGS, "_encode()"},
    {"_solve_inverse_matrix", (PyCFunction )Par2__solve_inverse_matrix, \
        METH_VARARGS, "_solve_inverse_matrix()"},
    {"_make_gf_and_gfi", (PyCFunction )Par2__make_gf_and_gfi, \
        METH_NOARGS, "_make_gf_and_gfi()"},
    {"_make_vandermonde_matrix", (PyCFunction )Par2__make_vandermonde_matrix, \
        METH_NOARGS, "_make_vandermonde_matrix()"},
    {"_make_square_matrix", (PyCFunction )Par2__make_square_matrix, \
        METH_NOARGS, "_make_square_matrix()"},
    {"_make_e_matrix", (PyCFunction )Par2__make_e_matrix, \
        METH_NOARGS, "_make_e_matrix()"},
    {"_mul_matrixes", (PyCFunction )Par2__mul_matrixes, \
        METH_VARARGS, "_mul_matrixes()"},
    {"_get_vandermonde_matrix", (PyCFunction )Par2__get_vandermonde_matrix, \
        METH_NOARGS, "_get_vandermonde_matrix()"},
    {"_add", (PyCFunction )Par2__add, METH_VARARGS, "_add()"},
    {"_mul", (PyCFunction )Par2__mul, METH_VARARGS, "_mul()"},
    {"_div", (PyCFunction )Par2__div, METH_VARARGS, "_div()"},
    /*
    */
    {"_pow", (PyCFunction )Par2__pow, METH_VARARGS, "_pow()"},
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

static PyTypeObject PyPar2Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_par2._Par2",             /* tp_name */
    sizeof(PyPar2Object),             /* tp_basicsize */
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

    if (PyType_Ready(&PyPar2Type) < 0)
        return NULL;

    m = PyModule_Create(&Par2_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyPar2Type);
    PyDict_SetItemString(PyPar2Type.tp_dict, "C_EXTENSION", Py_True);

    PyModule_AddObject(m, "_Par2", (PyObject *)&PyPar2Type);
/*
fprintf(stderr, "PyPar2Type.tp_dict = %p\n", PyPar2Type.tp_dict);
PyObject_Print(PyPar2Type.tp_dict, stderr, 0);
fprintf(stderr, "\n");
*/

    return m;
}
