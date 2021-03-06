/* author: 梅濁酒(umedoblock)
 * Copyright 平成23-24(2011-2012)年
 */
#include "pypar2.h"

static PyTypeObject PyPar2Type;
PyObject *pypar2_Error;
PyObject *pypar2_Par2Error;

static PyMemberDef Par2_members[] = {
// #define offsetof(type, member) ( (int) & ((type*)0) -> member )
    {"division", T_INT, offsetof(PyPar2Object, par2.division), 0, ""},
    {"object_used_size", T_UINT, \
        offsetof(PyPar2Object, object_used_size), 0, ""},
    {"vertical_size", T_INT, offsetof(PyPar2Object, par2.vertical_size), 0, ""},
    {"horizontal_size", T_INT, \
        offsetof(PyPar2Object, par2.horizontal_size), 0, ""},
    {NULL}  // Sentinel
};

static int
Par2_init(PyPar2Object *self, PyObject *args, PyObject *kwds);

static PyObject *
Par2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyPar2Object *self = NULL;
    int ret = -1;

/*
fprintf(stderr, "Par2_new(type=%p, args=%p, kwds=%p)\n", type, args, kwds);
fprintf(stderr, "\n");
*/
    self = (PyPar2Object *)type->tp_alloc(type, 0);
    if (self != NULL) {
        ret = Par2_init(self, args, kwds);
        if (ret < 0) {
            Py_DECREF(self);
            self = NULL;
        }
    }

    return (PyObject *)self;
}

static int
Par2_init(PyPar2Object *self, PyObject *args, PyObject *kwds)
{
    par2_t *p2 = &self->par2;
    int ret, bits = -1, division = -1, max_division = -1;
    char *kwlist[] = {"bits", "division", NULL};
    char errmsg[80];

/*
fprintf(stderr, "Par2_init(self=%p, args=%p, kwds=%p)\n", self, args, kwds);
*/
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii",
                                     kwlist, &bits, &division))
        return -1;

        // fprintf(stderr, "division is %d, bits = %d\n", division, bits);
    ret = par2_init_p2(p2, bits, division, errmsg);
    if (division > PYPAR2_MAX_REDUNDANCY || ret < 0) {
        if (p2->rds && p2->rds->gf_max > PYPAR2_MAX_REDUNDANCY) {
            max_division = PYPAR2_MAX_REDUNDANCY;
            sprintf(errmsg, "division(=%d) must be 2 <= division <= %d.",
                                        division, max_division);
        }
        else if (ret == PAR2_INVALID_BITS_ERROR ||
             ret == PAR2_INVALID_REDUNDANCY_ERROR ) {
             errmsg[strlen(errmsg)-1] = '\0';
        }
        else {
            sprintf(errmsg, "unknown return code that is %d.", ret);
        }
        PyErr_Format(pypar2_Par2Error, errmsg);

        /*
        par2_view_p2(p2);
        */
        return ret;
    }
    self->object_used_size = sizeof(PyPar2Object) + p2->allocate_size;

    return 0;
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
    reed_solomon_t *rds = p2->rds;
    ushort a = 0, b = 0, c = 0;
    uint a32 = 0, b32 = 0, c32 = 0;

    if (rds->code_size == 2) {
        if (!PyArg_ParseTuple(args, "HH", &a, &b))
            return NULL;
        c = par2_ADD(a, b);
        return Py_BuildValue("H", c);
    }
    else {
        if (!PyArg_ParseTuple(args, "II", &a32, &b32))
            return NULL;
        c32 = par2_ADD(a32, b32);
        return Py_BuildValue("I", c32);
    }
}

static PyObject *
Par2__mul(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = p2->rds;
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
    reed_solomon_t *rds = p2->rds;
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
    reed_solomon_t *rds = p2->rds;
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
/*
fprintf(stderr, "Par2__make_gf_and_gfi(self=%p)\n", self);
*/
    par2_make_gf_and_gfi(p2->rds);

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

    Py_MEMCPY(vm.ptr, p2->vandermonde_matrix.ptr, p2->matrix_size);

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
    reed_solomon_t *rds = p2->rds;
    PyObject *parity_slots_obj, *data_slots_obj, *slot_obj;
    int symbol_num;
    int len_data_slots, len_parity_slots;
    int i;

    if (!PyArg_ParseTuple(args, "OOi", \
                          &parity_slots_obj, &data_slots_obj, &symbol_num))
        return NULL;

    len_data_slots = PySequence_Length(data_slots_obj);
    len_parity_slots = PySequence_Length(parity_slots_obj);

    for (i=0;i<len_data_slots;i++) {
        slot_obj = PySequence_GetItem(data_slots_obj, i);
        p2->data_slots[i].u8 = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    for (i=0;i<len_parity_slots;i++) {
        slot_obj = PySequence_GetItem(parity_slots_obj, i);
        p2->parity_slots[i].u8 = (uchar *)PyByteArray_AS_STRING(slot_obj);
    }

    if (rds->code_size == 2) {
        par2_encode(p2, symbol_num);
    }
    else {
        par2_encode32(p2, symbol_num);
    }

    Py_RETURN_NONE;
}

static PyObject *
Par2__decode(PyPar2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    reed_solomon_t *rds = p2->rds;
    PyObject *slot_obj, *decode_data_slots_obj, *merged_slots_obj;
    PyBytesObject *inverse_matrix_obj = NULL;
    int symbol_num;
    int len_decode_data_slots, len_merged_slots;
    ptr_t inverse_matrix;
    int i;

    if (!PyArg_ParseTuple(args, "OOOi", \
                          &decode_data_slots_obj, &merged_slots_obj, \
                          &inverse_matrix_obj, &symbol_num))
        return NULL;

    inverse_matrix.ptr = (char *)PyBytes_AS_STRING(inverse_matrix_obj);

    len_decode_data_slots = PySequence_Length(decode_data_slots_obj);
    len_merged_slots = PySequence_Length(merged_slots_obj);

    for (i=0;i<len_decode_data_slots;i++) {
        slot_obj = PySequence_GetItem(decode_data_slots_obj, i);
        p2->data_slots[i].u8 = (uchar *)PyByteArray_AS_STRING(slot_obj);
    }

    for (i=0;i<len_merged_slots;i++) {
        slot_obj = PySequence_GetItem(merged_slots_obj, i);
        p2->merged_slots[i].u8 = (uchar *)PyBytes_AS_STRING(slot_obj);
    }

    if (rds->code_size == 2) {
        par2_decode(p2, inverse_matrix, symbol_num);
    }
    else {
        par2_decode32(p2, inverse_matrix, symbol_num);
    }

    Py_RETURN_NONE;
}

static int _set_slots(slot_t *slots, PyListObject *slots_obj, int len_slots)
{
    int i, count = 0;
    PyObject *slot_obj;

    for (i=0;i<len_slots;i++) {
        slot_obj = PySequence_GetItem((PyObject *)slots_obj, i);
        if (PyObject_IsTrue(slot_obj)) {
            slots[i].ptr = (void *)PyBytes_AS_STRING(slot_obj);
            count++;
        }
        else{
            slots[i].ptr = NULL;
        }
    }

    return count;
}

static int _set_merged_slots(PyTupleObject *merged_slots_obj, \
                             PyListObject *data_slots_obj, \
                             PyListObject *parity_slots_obj, \
                             par2_t *p2)
{
    PyObject *slot_obj;

    int data_slot_index, parity_slot_index;
    int i, count = 0;

    for (i=0;i<p2->division;i++) {
        slot_obj = NULL;

        data_slot_index = \
            par2_get_index_of_slots(p2, p2->data_slots, p2->merged_slots[i]);
        if (data_slot_index >= 0) {
            slot_obj = PySequence_GetItem( \
                            (PyObject *)data_slots_obj, data_slot_index);
        }

        parity_slot_index = \
            par2_get_index_of_slots(p2, p2->parity_slots, p2->merged_slots[i]);
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
    int len_slots, len_data_slots, len_parity_slots;
    ptr_t merged_matrix;

    PyTupleObject *merged_slots_obj = NULL;
    PyBytesObject *merged_matrix_obj = NULL;
    PyListObject *data_slots_obj, *parity_slots_obj;

    if (!PyArg_ParseTuple(args, "OO", \
                          &data_slots_obj, &parity_slots_obj))
        return NULL;

    len_data_slots = PySequence_Length((PyObject *)data_slots_obj);
    len_parity_slots = PySequence_Length((PyObject *)parity_slots_obj);
    len_slots = len_data_slots + len_parity_slots;
    if (len_slots < p2->division * 2) {
        /* to avoid noisy compiler */
    }

    merged_matrix_obj = \
        (PyBytesObject *)PyBytes_FromStringAndSize((const char *)NULL, \
                                                    p2->matrix_size);

    _set_slots(p2->data_slots, data_slots_obj, len_data_slots);
    _set_slots(p2->parity_slots, parity_slots_obj, len_parity_slots);

    merged_matrix.ptr = (void *)PyBytes_AS_STRING(merged_matrix_obj);
    par2_merge_slots(p2, merged_matrix);

    merged_slots_obj = (PyTupleObject *)PyTuple_New(p2->division);

    _set_merged_slots(merged_slots_obj, \
                      data_slots_obj, \
                      parity_slots_obj, \
                      p2);

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
    if (ret < 0) {
        Py_DECREF(inverse_matrix);
        inverse_matrix = NULL;
        if (ret == PAR2_RANK_ERROR){
            /* Par2Error */
            PyErr_Format(pypar2_Par2Error,
                "cannot make inverse_matrix. bits = %d, division = %d.",
                                             p2->rds->bits, p2->division);
        }
        else {
            PyErr_Format(pypar2_Par2Error,
                "unknown return code %d in solve_inverse_matrix().", ret);
        }
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
    PyObject *m, *dict;
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

    dict = PyModule_GetDict(m);
    if (dict == NULL) {
        goto error;
    }
/*
PyDict_DelItemString(PyPar2Type.tp_dict, "big_bang_obj");
*/
        // PyErr_NewException("_par2.Par2Error", PyExc_Exception, NULL);
    pypar2_Error =
        PyErr_NewException("_par2.Error", PyExc_BaseException, NULL);
    if (pypar2_Error == NULL)
        goto error;
    PyDict_SetItemString(dict, "Error", pypar2_Error);

    pypar2_Par2Error =
        PyErr_NewException("_par2.Par2Error", pypar2_Error, NULL);
    if (pypar2_Par2Error == NULL) {
        goto error;                                                                 }
    PyDict_SetItemString(dict, "Par2Error", pypar2_Par2Error);

/*
fprintf(stderr, "PyPar2Type.tp_dict = %p\n", PyPar2Type.tp_dict);
_PyObject_Dump((PyObject *)PyPar2Type.tp_dict);
fprintf(stderr, "\n");
*/

error:
    if (PyErr_Occurred()) {
        PyErr_SetString(PyExc_ImportError, "Par2: init failed");
        Py_DECREF(m);
        m = NULL;
    }

    return m;
}

PyMODINIT_FUNC
PyInit__par2_pg(void)
{
    return PyInit__par2();
}
