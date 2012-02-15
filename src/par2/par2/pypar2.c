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
    int matrix_size;
    int vertical_size;
    int horizontal_size;
    ushort *gf;
    ushort *gfi;
    ushort *vander_matrix;
    ushort *inverse_matrix;
    ushort *horizontal_vector;
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
    {"horizontal_size", T_INT, \
        offsetof(Par2Object, par2.horizontal_size), 0, ""},
    {NULL}  // Sentinel
};

static PyObject *
Par2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Par2Object *self = NULL;

fprintf(stderr, "Par2_new(type=%p, args=%p, kwds=%p)\n", type, args, kwds);
    self = (Par2Object *)type->tp_alloc(type, 0);
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
Par2_init(Par2Object *self, PyObject *args, PyObject *kwds)
{
    int gf_size = 0, matrix_size = 0;
    int allocate_size = 0, horizontal_size = 0;
    char *mem;

fprintf(stderr, "Par2_init(self=%p, args=%p, kwds=%p)\n", self, args, kwds);
    gf_size = self->par2.w * sizeof(ushort);
    matrix_size = \
        self->par2.redundancy * self->par2.redundancy * sizeof(ushort);
    self->par2.matrix_size = matrix_size;
    horizontal_size = sizeof(ushort) * self->par2.redundancy;
    self->par2.horizontal_size = horizontal_size;
    /*  need two matrixes.
        vander_matrix, inverse_matrix
     */
    allocate_size = gf_size * 2 + matrix_size * 2 + horizontal_size;
    self->mem = PyMem_Malloc(allocate_size);
/*
fprintf(stderr, "gf_size = %d\n", gf_size);
fprintf(stderr, "matrix_size = %d\n", matrix_size);
fprintf(stderr, "horizontal_size = %d\n", horizontal_size);
fprintf(stderr, "PyMem_Malloc(allocate_size=%d) = %p\n", \
                                    allocate_size, self->mem);
*/
    if (self->mem == NULL)
        return -1;

    mem = self->mem;
    self->par2.gf = (ushort *)mem; mem += gf_size;
    self->par2.gfi = (ushort *)mem; mem += gf_size;
    self->par2.vander_matrix = (ushort *)mem; mem += matrix_size;
    self->par2.inverse_matrix = (ushort *)mem; mem += matrix_size;
    self->par2.horizontal_vector = (ushort *)mem; mem += horizontal_size;

/*
fprintf(stderr, "self->par2.poly=%d\n", self->par2.poly);
fprintf(stderr, "self->par2.bits=%d\n", self->par2.bits);
fprintf(stderr, "self->par2.w=%d\n", self->par2.w);
fprintf(stderr, "self->par2.gf_max = %d\n", self->par2.gf_max);
fprintf(stderr, "self->par2.digits = %d\n", self->par2.digits);
fprintf(stderr, "self->par2.redundancy = %d\n", self->par2.redundancy);
fprintf(stderr, "self->par2.octets = %d\n", self->par2.octets);
fprintf(stderr, "self->par2.vertical_size = %d\n", self->par2.vertical_size);

fprintf(stderr, "dir(self)=\n");
PyObject_Dir((PyObject *)self);

fprintf(stderr, "\n");
*/

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
/*
fprintf(stderr, "Par2_free(self=%p)\n", self);
*/
    PyObject_Free(self);
}

static ushort
_add(ushort a, ushort b)
{
    int c;

    c = a ^ b;

    return c;
}

static PyObject *
Par2__add(Par2Object *self, PyObject *args)
{
    ushort a, b, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &b))
        return NULL;

    c = _add(a, b);

    return Py_BuildValue("H", c);
}

static ushort
_mul(Par2Object *self, ushort a, ushort b)
{
    int c;
    par2_t *p2 = &self->par2;

    if (a == 0 || b == 0)
        return 0;

    c = p2->gf[a] + p2->gf[b];

    if (c < p2->gf_max)
        return p2->gfi[c];
    return p2->gfi[c - p2->gf_max];
}

static PyObject *
Par2__mul(Par2Object *self, PyObject *args)
{
    ushort a, b, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &b))
        return NULL;

    c = _mul(self, a, b);

    return Py_BuildValue("H", c);
}

static ushort
_div(Par2Object *self, ushort a, ushort b)
{
    int c;
    par2_t *p2 = &self->par2;

    if (a == 0)
        return 0;

    c = p2->gf[a] - p2->gf[b];
    if (c >= 0)
        return p2->gfi[c];
    return p2->gfi[c + p2->gf_max];
}

static PyObject *
Par2__div(Par2Object *self, PyObject *args)
{
    ushort a, b, c;

    if (!PyArg_ParseTuple(args, "HH", &a, &b))
        return NULL;

    c = _div(self, a, b);

    return Py_BuildValue("H", c);
}

static PyObject *
Par2__make_gf_and_gfi(Par2Object *self)
{
    int i, bit_pattern = 1;
    par2_t *p2 = &self->par2;
    ushort *gf = p2->gf, *gfi = p2->gfi;

/*
fprintf(stderr, "Par2__make_gf_and_gfi(self=%p)\n", self);
*/
    for(i=0;i<p2->gf_max;i++){
        if (bit_pattern & p2->w)
            bit_pattern ^= p2->poly;
        gf[bit_pattern] = i;
        gfi[i] = bit_pattern;
        bit_pattern <<= 1;
    }

    Py_RETURN_NONE;
}

static PyObject *
Par2__get_vandermonde_matrix(Par2Object *self)
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

    memcpy(vm, p2->vander_matrix, p2->matrix_size);

    return (PyObject *)vandermonde_matrix;
}

static PyObject *
Par2__make_vandermonde_matrix(Par2Object *self)
{
    par2_t *p2 = &self->par2;
    ushort *vm = p2->vander_matrix;
    int i, j, redundancy = p2->redundancy;

/*
fprintf(stderr, "Par2__make_vandermonde_matrix(self=%p)\n", self);
*/
    for(i=0;i<redundancy;i++)
        vm[i] = 1;

    for(j=1;j<redundancy;j++)
        for(i=0;i<redundancy;i++)
            vm[j * redundancy + i] = \
                _mul(self, vm[(j-1) * redundancy + i], i + 1);

    Py_RETURN_NONE;
}

static PyBytesObject *
Par2__make_square_matrix(Par2Object *self)
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
Par2__make_e_matrix(Par2Object *self)
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

void
view_matrix(ushort *matrix, int redundancy)
{
    int i, j;

fprintf(stderr, "view_matrix(matrix=%p, redundancy=%d\n", matrix, redundancy);
    fprintf(stderr, "     ");
    for (i=0;i<redundancy;i++)
        fprintf(stderr, "%4x ", i);
    fprintf(stderr, "\n");
    for (j=0;j<redundancy;j++) {
        fprintf(stderr, "%4x ", j);
        for (i=0;i<redundancy;i++) {
            fprintf(stderr, "%4x ", matrix[j * redundancy + i]);
        }
        fprintf(stderr, "\n");
    }
}

static int
_solve_inverse_matrix(ushort *inverse_matrix, ushort *matrix, \
                      int redundancy, ushort *horizontal_vector, \
                      Par2Object *self)
{
    int i, j, k, swap;
    int index, index_j, index_k, index_x, index_y, index_z;
    int horizontal_size = redundancy * sizeof(ushort);
    ushort *im = inverse_matrix, *mt = matrix, *hv = horizontal_vector;
    ushort tmp, tmp1, tmp2, tmp3;

/*
view_matrix(matrix, redundancy);
view_matrix(inverse_matrix, redundancy);
*/

    for (k=0;k<redundancy;k++) {
        if (mt[k * redundancy + k] == 0) {
            swap = 0;
            for (j=k+1;j<redundancy;j++) {
                if (mt[j * redundancy + k]) {
                    swap = 1;
fprintf(stderr, "matrix[%d], matrix[%d] = matrix[%d], matrix[%d]\n", \
                        k, j, j, k);
                    /*
                    matrix[k], matrix[j] = matrix[j], matrix[k]
                    im[k], im[j] = im[j], im[k]
                    */
                    index_j = j * redundancy;
                    index_k = k * redundancy;
                    memcpy(hv, mt + index_j, horizontal_size);
                    memcpy(mt + index_j, mt + index_k, horizontal_size);
                    memcpy(mt + index_k, hv, horizontal_size);
                    memcpy(hv, im + index_j, horizontal_size);
                    memcpy(im + index_j, im + index_k, horizontal_size);
                    memcpy(im + index_k, hv, horizontal_size);
                    break;
                }
            }

            if (swap == 0) {
//              message =
//                  ('cannot make inverse_matrix. redundancy = {}, ',
//                   'gf_max = {}').format(self.redundancy, self.gf_max)
//              raise Par2RankError(message)
                return -1;
            }
        }

        tmp = matrix[k * redundancy + k];
        if (tmp != 1) {
            for (i=0;i<redundancy;i++){
                index = k * redundancy + i;
                mt[index] = _div(self, mt[index], tmp);
                im[index] = _div(self, im[index], tmp);
            }
        }

        for (j=k+1;j<redundancy;j++){
            tmp = mt[j * redundancy + k];
            if (tmp == 0)
                continue;
            for (i=0;i<redundancy;i++) {
                index_k = k * redundancy + i;
                index_j = j * redundancy + i;
/*
*/
                tmp1 = mt[index_k];
                tmp2 = _mul(self, tmp, tmp1);
                tmp3 = mt[index_j];
                mt[index_j] = _add(tmp3, tmp2);

                tmp1 = im[index_k];
                tmp2 = _mul(self, tmp, tmp1);
                tmp3 = im[index_j];
                im[index_j] = _add(tmp3, tmp2);
            }
        }
    }
/*
fprintf(stderr, "前進完了\n");
moving front done
*/

    for (k=0;k<redundancy-1;k++) {
        for (j=0;j<redundancy-1-k;j++) {
            index_z = redundancy - 1 - k;
            index_x = redundancy - 1 - k;
            index_y = redundancy - 1 - k - j - 1;
            tmp = mt[index_y * redundancy + index_x];

            for (i=0;i<redundancy;i++) {
/*
                tmp1 = mt[index_z * redundancy + i];
                tmp2 = _mul(self, tmp, tmp1);
                tmp3 = mt[index_y * redundancy + i];
                mt[index_y * redundancy + i] = _add(tmp3, tmp2);
*/
                tmp1 = im[index_z * redundancy + i];
                tmp2 = _mul(self, tmp, tmp1);
                tmp3 = im[index_y * redundancy + i];
                im[index_y * redundancy + i] = _add(tmp3, tmp2);
            }
        }
    }

    return 0;
}

static PyObject *
Par2__solve_inverse_matrix(Par2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    int redundancy = p2->redundancy;
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

    ret = _solve_inverse_matrix(im, mt, redundancy, \
                                p2->horizontal_vector, self);
    if (ret < 0){
        /* Par2RankError */
fprintf(stderr, "Par2RankError\n");
        return NULL;
    }

    return (PyObject *)inverse_matrix;
}

static PyObject *
Par2__mul_matrixes(Par2Object *self, PyObject *args)
{
    par2_t *p2 = &self->par2;
    int redundancy = p2->redundancy;
    int i, j, k;
    PyBytesObject *answer = NULL, *a, *b;
    ushort *ans, *a_mt, *b_mt, tmp, muled;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

    answer = Par2__make_square_matrix(self);
    if (answer == NULL)
        return NULL;
    ans = (ushort *)PyBytes_AS_STRING(answer);

    a_mt = (ushort *)PyBytes_AS_STRING(a);
    b_mt = (ushort *)PyBytes_AS_STRING(b);

    for (j=0;j<redundancy;j++){
        for (i=0;i<redundancy;i++){
            tmp = 0x0000;
            for (k=0;k<redundancy;k++){
                muled = _mul(self, a_mt[j * redundancy + k], \
                                   b_mt[k * redundancy + i]);
                tmp = _add(tmp, muled);
            }
            ans[j * redundancy + i] = tmp;
        }
    }

    return (PyObject *)answer;
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
/*
fprintf(stderr, "Par2_Type.tp_dict = %p\n", Par2_Type.tp_dict);
PyObject_Print(Par2_Type.tp_dict, stderr, 0);
fprintf(stderr, "\n");
*/

    return m;
}
