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
    {"poly", T_INT, offsetof(PyPar2Object, par2.reed_solomon.poly), 0, ""},
    {"bits", T_INT, offsetof(PyPar2Object, par2.reed_solomon.bits), 0, ""},
    {"w", T_INT, offsetof(PyPar2Object, par2.reed_solomon.w), 0, ""},
    {"gf_max", T_INT, offsetof(PyPar2Object, par2.reed_solomon.gf_max), 0, ""},
    {"digits", T_INT, offsetof(PyPar2Object, par2.digits), 0, ""},
    {"redundancy", T_INT, offsetof(PyPar2Object, par2.redundancy), 0, ""},
    {"octets", T_INT, offsetof(PyPar2Object, par2.reed_solomon.octets), 0, ""},
    {"code_size", T_INT, offsetof(PyPar2Object, par2.reed_solomon.code_size), 0, ""},
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
    reed_solomon_t *rds = &p2->reed_solomon;
    int ret;

fprintf(stderr, "Par2_init(self=%p, args=%p, kwds=%p)\n", self, args, kwds);
/*
*/
    ret = par2_init_structure(p2, rds->bits, p2->redundancy);
    if (ret < 0){
        par2_view_structure(p2);
        return -1;
    }
    p2->mem = NULL;

    return 0;
}

static PyObject *
Par2__allocate_memory(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    int ret;
/*
fprintf(stderr, "Par2__allocate_memory(self=%p)\n", self);
*/
    ret = par2_allocate_memory(p2);
    if (ret < 0) {
        par2_view_structure(p2);
        if (ret == PAR2_INVALID_REDUNDANCY_ERROR) {
            PyErr_SetString(PyExc_RuntimeError,
                            "numerous redundancy");
            return NULL;
        }
        else if (ret == PAR2_MALLOC_ERROR)
            return PyErr_NoMemory();
        else {
            PyErr_SetString(PyExc_RuntimeError,
                            "unknown return code");
            return NULL;
        }
    }
    p2->object_size = sizeof(PyPar2Object) + p2->allocate_size;
    /*
    par2_view_structure(p2);
    */

    Py_RETURN_NONE;
}

static void
Par2_dealloc(PyPar2Object* self)
{
    par2_t *p2 = &self->par2;
/*
fprintf(stderr, "Par2_dealloc(self=%p)\n", self);
*/
    if (p2->mem != NULL)
        par2_free_memory(p2);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static void
Par2_free(PyPar2Object* self)
{
fprintf(stderr, "Par2_free(self=%p)\n", self);
/*
*/
    PyObject_Free(self);
}

static PyObject *
Par2__add(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = &p2->reed_solomon;
    ushort a = 0, b = 0, c = 0;
    uint a32 = 0, b32 = 0, c32 = 0;

    if (rds->code_size == 2) {
        if (!PyArg_ParseTuple(args, "HH", &a, &b))
            return NULL;
        c = par2_add(a, b);
        return Py_BuildValue("H", c);
    }
    else {
        if (!PyArg_ParseTuple(args, "II", &a32, &b32))
            return NULL;
        c32 = par2_add(a32, b32);
        return Py_BuildValue("I", c32);
    }
}

static PyObject *
Par2__mul(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = &p2->reed_solomon;
    ushort a = 0, b = 0, c = 0;
    uint a32 = 0, b32 = 0, c32 = 0;

    if (rds->code_size == 2) {
        if (!PyArg_ParseTuple(args, "HH", &a, &b))
            return NULL;
        c = par2_mul(rds, a, b);
        return Py_BuildValue("H", c);
    }
    else {
        if (!PyArg_ParseTuple(args, "II", &a32, &b32))
            return NULL;
        c32 = par2_mul32(rds, a32, b32);
        return Py_BuildValue("I", c32);
    }
}

static PyObject *
Par2__div(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = &p2->reed_solomon;
    ushort a = 0, b = 0, c = 0;
    uint a32 = 0, b32 = 0, c32 = 0;

    if (rds->code_size == 2) {
        if (!PyArg_ParseTuple(args, "HH", &a, &b))
            return NULL;
        c = par2_div(rds, a, b);
        return Py_BuildValue("H", c);
    }
    else {
        if (!PyArg_ParseTuple(args, "II", &a32, &b32))
            return NULL;
        c32 = par2_div32(rds, a32, b32);
        return Py_BuildValue("I", c32);
    }
}
/*
*/

static PyObject *
Par2__pow(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = &p2->reed_solomon;
    ushort a = 0, x = 0, c = 0;
    uint a32 = 0, x32 = 0, c32 = 0;

    if (rds->code_size == 2) {
        if (!PyArg_ParseTuple(args, "HH", &a, &x))
            return NULL;
        c = par2_pow(rds, a, x);
        return Py_BuildValue("H", c);
    }
    else {
        if (!PyArg_ParseTuple(args, "II", &a32, &x32))
            return NULL;
        c32 = par2_pow32(rds, a32, x32);
        return Py_BuildValue("I", c32);
    }
}

static PyObject *
Par2__make_gf_and_gfi(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = &p2->reed_solomon;
/*
fprintf(stderr, "Par2__make_gf_and_gfi(self=%p)\n", self);
*/
    par2_make_gf_and_gfi(rds);

    Py_RETURN_NONE;
}

static PyObject *
Par2__get_vandermonde_matrix(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    ptr_t vm;
    PyBytesObject *vandermonde_matrix = NULL;

/*
fprintf(stderr, "Par2__get_vandermonde_matrix(self=%p)\n", self);
*/
    vandermonde_matrix = \
        (PyBytesObject *)PyBytes_FromStringAndSize(NULL, p2->matrix_size);
    if (vandermonde_matrix == NULL)
        return NULL;
    vm.ptr = (void *)PyBytes_AS_STRING(vandermonde_matrix);

    memcpy(vm.ptr, p2->vandermonde_matrix.ptr, p2->matrix_size);

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
    PyBytesObject *matrix = NULL;

    matrix = (PyBytesObject *)PyBytes_FromStringAndSize(NULL, p2->matrix_size);

    return matrix;
}

static PyObject *
Par2__make_e_matrix(PyPar2Object *self)
{
    par2_t *p2 = &self->par2;
    PyBytesObject *matrix = NULL;
    ptr_t mt;

    matrix = Par2__make_square_matrix(self);
    if (matrix == NULL)
        return NULL;
    mt.ptr = (void *)PyBytes_AS_STRING(matrix);

    par2_make_e_matrix(p2, mt);

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

    for (i=0;i<len_data_slots;i++) {
        slot_obj = PySequence_GetItem(data_slots_obj, i);
        data_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    for (i=0;i<len_parity_slots;i++) {
        slot_obj = PySequence_GetItem(parity_slots_obj, i);
        parity_slots[i] = (uchar *)PyByteArray_AS_STRING(slot_obj);
    }

    par2_encode(p2, parity_slots, data_slots, symbol_num);

    for (i=0;i<len_data_slots;i++) {
        slot_obj = PySequence_GetItem(data_slots_obj, i);
        data_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    for (i=0;i<len_parity_slots;i++) {
        slot_obj = PySequence_GetItem(parity_slots_obj, i);
        parity_slots[i] = (uchar *)PyByteArray_AS_STRING(slot_obj);
    }

    PyMem_Free(slots);

    Py_RETURN_NONE;
}

static PyObject *
Par2__decode(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    PyObject *slot_obj, *decode_data_slots_obj, *merged_slots_obj;
    PyBytesObject *inverse_matrix_obj = NULL;
    int allocate_size, symbol_num;
    int len_slots, len_decode_data_slots, len_merged_slots;
    uchar **slots, **decode_data_slots, **merged_slots;
    ptr_t inverse_matrix;
    int i;

    if (!PyArg_ParseTuple(args, "OOOi", \
                          &decode_data_slots_obj, &merged_slots_obj, \
                          &inverse_matrix_obj, &symbol_num))
        return NULL;

    inverse_matrix.ptr = (char *)PyBytes_AS_STRING(inverse_matrix_obj);

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
    decode_data_slots = slots;
    merged_slots = slots + p2->redundancy;
    len_decode_data_slots = PySequence_Length(decode_data_slots_obj);
    len_merged_slots = PySequence_Length(merged_slots_obj);
    len_slots = len_decode_data_slots + len_merged_slots;
    if (len_slots < p2->redundancy * 2) {
        /* to avoid noisy compiler */
    }

    for (i=0;i<len_decode_data_slots;i++) {
        slot_obj = PySequence_GetItem(decode_data_slots_obj, i);
        decode_data_slots[i] = (uchar *)PyByteArray_AS_STRING(slot_obj);
    }

    for (i=0;i<len_merged_slots;i++) {
        slot_obj = PySequence_GetItem(merged_slots_obj, i);
        merged_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    par2_decode(p2, decode_data_slots, merged_slots, \
                    inverse_matrix, symbol_num);

    for (i=0;i<len_decode_data_slots;i++) {
        slot_obj = PySequence_GetItem(decode_data_slots_obj, i);
        decode_data_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    for (i=0;i<len_merged_slots;i++) {
        slot_obj = PySequence_GetItem(merged_slots_obj, i);
        merged_slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    PyMem_Free(slots);

    Py_RETURN_NONE;
}

static int _set_slots(uchar **slots, PyListObject *slots_obj, int len_slots)
{
    int i, count = 0;
    PyObject *slot_obj;

    for (i=0;i<len_slots;i++) {
        slot_obj = PySequence_GetItem((PyObject *)slots_obj, i);
        if (PyObject_IsTrue(slot_obj)) {
            slots[i] = (uchar *)PyBytes_AS_STRING(slot_obj);
            count++;
        }
        else{
            slots[i] = NULL;
        }
    }

    return count;
}

static int _set_merged_slots(PyTupleObject *merged_slots_obj, \
                             PyListObject *data_slots_obj, \
                             PyListObject *parity_slots_obj, \
                             uchar **merged_slots, \
                             uchar **data_slots, \
                             uchar **parity_slots, \
                             par2_t *p2)
{
    PyObject *slot_obj;

    int data_slot_index, parity_slot_index;
    int i, count = 0;

    for (i=0;i<p2->redundancy;i++) {
        slot_obj = NULL;

        data_slot_index = \
            par2_get_index_of_slots(p2, data_slots, merged_slots[i]);
        if (data_slot_index >= 0) {
            slot_obj = PySequence_GetItem( \
                            (PyObject *)data_slots_obj, data_slot_index);
        }

        parity_slot_index = \
            par2_get_index_of_slots(p2, parity_slots, merged_slots[i]);
        if (parity_slot_index >= 0) {
            slot_obj = \
                PySequence_GetItem( \
                    (PyObject *)parity_slots_obj, parity_slot_index);
        }

        if (slot_obj == NULL)
            break;

        PyTuple_SetItem((PyObject *)merged_slots_obj, i, slot_obj);
        count++;
    }

    return count;
}

static PyObject *
Par2__merge_slots(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    int allocate_size;
    int len_slots, len_data_slots, len_parity_slots;
    ptr_t merged_matrix;
    uchar **slots, **merged_slots, **data_slots, **parity_slots;

    PyTupleObject *merged_slots_obj = NULL;
    PyBytesObject *merged_matrix_obj = NULL;
    PyListObject *data_slots_obj, *parity_slots_obj;

    if (!PyArg_ParseTuple(args, "OO", \
                          &data_slots_obj, &parity_slots_obj))
        return NULL;

    allocate_size = sizeof(uchar *) * p2->redundancy * 3;
    slots = (uchar **)PyMem_Malloc(allocate_size);
    if (slots == NULL)
        return NULL;

    data_slots = slots;
    parity_slots = slots + p2->redundancy;
    merged_slots = slots + p2->redundancy * 2;
    len_data_slots = PySequence_Length((PyObject *)data_slots_obj);
    len_parity_slots = PySequence_Length((PyObject *)parity_slots_obj);
    len_slots = len_data_slots + len_parity_slots;
    if (len_slots < p2->redundancy * 2) {
        /* to avoid noisy compiler */
    }

    merged_matrix_obj = \
        (PyBytesObject *)PyBytes_FromStringAndSize((const char *)NULL, \
                                                    p2->matrix_size);

    _set_slots(data_slots, data_slots_obj, len_data_slots);
    _set_slots(parity_slots, parity_slots_obj, len_parity_slots);

    merged_matrix.ptr = (void *)PyBytes_AS_STRING(merged_matrix_obj);
    par2_merge_slots(p2, \
                     merged_slots, merged_matrix, \
                     data_slots, parity_slots);

    merged_slots_obj = (PyTupleObject *)PyTuple_New(p2->redundancy);

    _set_merged_slots(merged_slots_obj, \
                      data_slots_obj, \
                      parity_slots_obj, \
                      merged_slots, \
                      data_slots, \
                      parity_slots, \
                      p2);
    PyMem_Free(slots);

    return PyTuple_Pack(2, merged_slots_obj, merged_matrix_obj);
}

static PyObject *
Par2__solve_inverse_matrix(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    int ret;
    PyBytesObject *inverse_matrix = NULL;
    Py_buffer matrix;
    ptr_t mt, im;

    if (!PyArg_ParseTuple(args, "y*", &matrix))
        return NULL;
    mt.ptr = (char *)matrix.buf;

    inverse_matrix = (PyBytesObject *)Par2__make_e_matrix(self);
    if (inverse_matrix == NULL)
        return NULL;
    im.ptr = (char *)PyBytes_AS_STRING(inverse_matrix);

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
    ptr_t ans, a_mt, b_mt;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

    answer = Par2__make_square_matrix(self);
    if (answer == NULL)
        return NULL;

    ans.ptr = (char *)PyBytes_AS_STRING(answer);
    a_mt.ptr = (char *)PyBytes_AS_STRING(a);
    b_mt.ptr = (char *)PyBytes_AS_STRING(b);

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
    {"_allocate_memory", (PyCFunction )Par2__allocate_memory, \
        METH_NOARGS, "_allocate_memory()"},
    {"_encode", (PyCFunction )Par2__encode, \
        METH_VARARGS, "_encode()"},
    {"_decode", (PyCFunction )Par2__decode, \
        METH_VARARGS, "_decode()"},
    {"_merge_slots", (PyCFunction )Par2__merge_slots, \
        METH_VARARGS, "_merge_slots()"},
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

typedef struct {
    PyObject_HEAD
} PyBigBangObject;

/*
typedef struct PyModuleDef{
  PyModuleDef_Base m_base;
  const char* m_name;
  const char* m_doc;
  Py_ssize_t m_size;
  PyMethodDef *m_methods;
  inquiry m_reload;
  traverseproc m_traverse;
  inquiry m_clear;
  freefunc m_free;
}PyModuleDef;

typedef int (*inquiry)(PyObject *);
typedef int (*traverseproc)(PyObject *, visitproc, void *);
typedef void (*freefunc)(void *);
*/

static int
BigBang_init(PyBigBangObject *self, PyObject *args, PyObject *kwds);

static PyObject *
BigBang_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyBigBangObject *self = NULL;
    int ret;

fprintf(stderr, "big baaaaaaaaaaaaaaaaaaaaaaaaaaan "
                "dokaaaaaaaaaaaaaaaaaaaaaaaaaaaan !!!!!!!!!!!\n");
/*
fprintf(stderr, "BigBang_new(type=%p, args=%p, kwds=%p)\n", type, args, kwds);
*/
    self = (PyBigBangObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        ret = BigBang_init(self, args, kwds);
        if (ret == PAR2_MALLOC_ERROR) {
            Py_DECREF(self);
            self = NULL;
        }
/*
fprintf(stderr, "self = %p\n", self);
*/
    }
/*
fprintf(stderr, "\n");
*/

    return (PyObject *)self;
}

static int
BigBang_init(PyBigBangObject *self, PyObject *args, PyObject *kwds)
{
    int done;

/*
fprintf(stderr, "BigBang_init(self=%p, args=%p, kwds=%p)\n", self, args, kwds);
*/
        done = par2_big_bang();
        if (done == PAR2_MALLOC_ERROR){
            /*
            fprintf(stderr, "par2_big_bang() failed.\n");
            */
            return PAR2_MALLOC_ERROR;
        }
        else {
            /*
            fprintf(stderr, "par2_big_bang() huge explosion!!!.\n");
            par2_view_big_bang();
            */
        }

    return 0;
}

static void
BigBang_free(PyBigBangObject* self)
{
fprintf(stderr, "BigBang_free(self=%p)\n", self);
/*
*/
    par2_ultimate_fate_of_the_universe();
    PyObject_Free(self);
}

static void
BigBang_dealloc(PyBigBangObject* self)
{
fprintf(stderr, "BigBang_dealloc(self=%p)\n", self);
/*
*/
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyTypeObject PyBigBangType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_bigbang._BigBang",             /* tp_name */
    sizeof(PyBigBangObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)BigBang_dealloc, /* tp_dealloc */
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
    "BigBang objects",           /* tp_doc */
    0,                     /* tp_traverse */
    0,                     /* tp_clear */
    0,                     /* tp_richcompare */
    0,                     /* tp_weaklistoffset */
    0,                     /* tp_iter */
    0,                     /* tp_iternext */
    0,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)BigBang_init,      /* tp_init */
    0,               /* tp_alloc */
    BigBang_new,                 /* tp_new */
    (freefunc)BigBang_free,                /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__par2(void)
{
    /* PyModuleObject *m; */
    PyObject *m;
    PyBigBangObject *big_bang_obj;

    if (PyType_Ready(&PyBigBangType) < 0)
        return NULL;
    if (PyType_Ready(&PyPar2Type) < 0)
        return NULL;
    /* _PyObject_Dump((PyObject *)&PyPar2Type); */

    m = (PyObject *)PyModule_Create(&Par2_module);
    if (m == NULL) {
        return NULL;
    }

    PyDict_SetItemString(PyPar2Type.tp_dict, "C_EXTENSION", Py_True);

    Py_INCREF(&PyBigBangType);
    PyModule_AddObject(m, "_BigBang", (PyObject *)&PyBigBangType);
    /*
    fprintf(stderr, "PyModule_AddObject()\n");
    fprintf(stderr, "PyBigBangType = %p\n", &PyBigBangType);
    _PyObject_Dump((PyObject *)&PyBigBangType);
    fprintf(stderr, "\n");
    */

    Py_INCREF(&PyPar2Type);
    PyModule_AddObject(m, "_Par2", (PyObject *)&PyPar2Type);

    big_bang_obj = (PyBigBangObject *)BigBang_new(&PyBigBangType, NULL, NULL);

    PyDict_SetItemString(PyPar2Type.tp_dict, "big_bang_obj",
                        (PyObject *)big_bang_obj);
    Py_DECREF(big_bang_obj);

/*
PyDict_DelItemString(PyPar2Type.tp_dict, "big_bang_obj");
*/

/*
fprintf(stderr, "PyPar2Type.tp_dict = %p\n", PyPar2Type.tp_dict);
_PyObject_Dump((PyObject *)PyPar2Type.tp_dict);
fprintf(stderr, "\n");
*/
    return m;
}
