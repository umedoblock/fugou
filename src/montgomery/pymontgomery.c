#include "Python.h"
#include "structmember.h"

#define MONTGOMERY_MODULE

static PyObject *MontgomeryErrorObject;     /* MontgomeryError exception */

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    PyObject *n;
    PyObject *nbit;
    PyObject *r;
    PyObject *r_square;
    PyObject *r_;
    PyObject *ffffffff;
    PyObject *n_;
    PyObject *c_extension;
} MontgomeryObject;

static PyMemberDef Montgomery_members[] = {
    {"n", T_OBJECT, offsetof(MontgomeryObject, n), 0,
        "modulus"},
    {"nbit", T_OBJECT, offsetof(MontgomeryObject, nbit), 0,
        "bit length of modulus"},
    {"r", T_OBJECT, offsetof(MontgomeryObject, r), 0,
        "2 ** nbit"},
    {"r_square", T_OBJECT, offsetof(MontgomeryObject, r_square), 0,
        "r * r mod n"},
    {"r_", T_OBJECT, offsetof(MontgomeryObject, r_), 0,
        "r * r_ = 1 mod n"},
    {"ffffffff", T_OBJECT, offsetof(MontgomeryObject, ffffffff), 0,
        "r - 1"},
    {"n_", T_OBJECT, offsetof(MontgomeryObject, n_), 0,
        "x * (r - 1) mod r"},
    {"c_extension", T_OBJECT, offsetof(MontgomeryObject, c_extension), 0,
        "use c extension or not"},
    {NULL}  // Sentinel
};

static PyObject *
Montgomery_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    MontgomeryObject *self = NULL;
    self = (MontgomeryObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->n = NULL;
        self->nbit = NULL;
        self->r = NULL;
        self->r_square = NULL;
        self->r_ = NULL;
        self->ffffffff = NULL;
        self->n_ = NULL;
        self->c_extension = NULL;
    }

    return (PyObject *)self;
}

static void
Montgomery_dealloc(MontgomeryObject* self)
{
    Py_XDECREF(self->n);
    Py_XDECREF(self->nbit);
    Py_XDECREF(self->r);
    Py_XDECREF(self->r_square);
    Py_XDECREF(self->r_);
    Py_XDECREF(self->ffffffff);
    Py_XDECREF(self->n_);
    Py_XDECREF(self->c_extension);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static size_t
_bit_length(PyLongObject *num)
{
    return _PyLong_NumBits((PyObject *)num);
}

static PyObject *
montgomery_gcdext(PyObject *self, PyObject *args)
{
    PyObject *ret, *a, *b;
    PyObject *x, *y, *gcd, *x1, *y1, *z1, *x2, *y2, *z2;
    PyObject *divmod, *div, *div_x1, *div_y1, *zero;
    int count = 0;

    x2 = y2 = z2 = NULL;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;
/*
fprintf(stderr, "a =\n");
PyObject_Print(a, stderr, 0);
fprintf(stderr, "\n");
fprintf(stderr, "b =\n");
PyObject_Print(b, stderr, 0);
fprintf(stderr, "\n");
*/

    // x, y, gcd = 1, 0, a
    x = PyLong_FromLong(1);
    y = PyLong_FromLong(0);
    gcd = Py_BuildValue("O", a);
    // x1, y1, z1 = 0, 1, b
    x1 = PyLong_FromLong(0);
    y1 = PyLong_FromLong(1);
    z1 = PyNumber_Long(b);

    zero = PyLong_FromLong(0);

//  while z1 > 0:
    while (PyObject_RichCompareBool(z1, zero, Py_GT) == 1) {
    //  div, z2 = divmod(gcd, z1)
        divmod = PyNumber_Divmod(gcd, z1);
        /*
if(count == 577){
fprintf(stderr, "gcd =\n");
PyObject_Print(gcd, stderr, 0);
fprintf(stderr, "\n");
fprintf(stderr, "z1 =\n");
PyObject_Print(z1, stderr, 0);
fprintf(stderr, "\n");
fprintf(stderr, "divmod =\n");
PyObject_Print(divmod, stderr, 0);
fprintf(stderr, "\n");
}
        */
        div = PyTuple_GetItem(divmod, 0);
        z2 = PySequence_GetItem(divmod, 1);
    //  div_x1 = div * x1
        div_x1 = PyNumber_Multiply(div, x1);
    //  x2 = x - div_x1
        x2 = PyNumber_Subtract(x, div_x1);
    //  div_y1 = div * y1
        div_y1 = PyNumber_Multiply(div, y1);
    //  y2 = y - div_y1
        y2 = PyNumber_Subtract(y, div_y1);

        Py_DECREF(div_x1);
        Py_DECREF(div_y1);
        Py_DECREF(divmod);

        Py_DECREF(x);
        Py_DECREF(y);
        Py_DECREF(gcd);
        /*
        Py_DECREF(gcd); を comment out していないと
        benchmark.py で落ちてしまう。
        落ちない状況で100回pow()を実行した場合の速度を
        計測してみたが、python scriptと同程度の速度だった。
        bugをとっても劇的な速度改善とはいえないので、
        おそらくpow()の中にあるであろうbug取りはしない。
        __builtins__.pow(a, x, n)
        2.70 usec/pass
        mng_py3.pow(a, x)
        6.24 usec/pass
        mng_c_ext.pow(a, x)
        6.08 usec/pass
        */

    //  x = x1
        x = x1;
    //  y = y1
        y = y1;
    //  gcd = z1
        // gcd = Py_BuildValue("O", z1);
        gcd = z1;
    //  x1 = x2 //  y1 = y2 //  z1 = z2
        x1 = x2; y1 = y2; z1 = z2;
/*
fprintf(stderr, "count = %d\n", count);
        count++;
*/
    }

//  if x < 0:
    if (PyObject_RichCompareBool(x, zero, Py_LT) == 1) {
//      x = b + x
        x1 = PyNumber_Add(b, x);
        Py_DECREF(x);
        x = x1;
//      y = a - y
        y1 = PyNumber_Subtract(a, y);
        Py_DECREF(y);
        y = y1;
    }
//  else:
    else{
//      y = abs(y)
        y1 = PyNumber_Absolute(y);
        Py_DECREF(y);
        y = y1;
    }
    ret = PyTuple_New(3);

    PyTuple_SetItem(ret, 0, x);
    PyTuple_SetItem(ret, 1, y);
    PyTuple_SetItem(ret, 2, gcd);

    if (x2)
        Py_DECREF(x2);
    if (y2)
        Py_DECREF(y2);
    if (z2)
        Py_DECREF(z2);
    Py_DECREF(zero);

    return ret;
}

static int
Montgomery_init(MontgomeryObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *zero = NULL, *one, *two, *rn, *xygcd, *x, *y;
    PyLongObject *n = NULL;
    int nbit, cmp, bool1, bool2;

    if (!PyArg_ParseTuple(args, "O", &n))
        return -1;
    self->n = (PyObject *)n;

    zero = PyLong_FromLong(0);
    cmp = PyObject_RichCompareBool(self->n, zero, Py_LE);
    // not isinstance(n, int) or
    bool1 = PyObject_IsInstance(self->n, (PyObject *)&PyLong_Type) == 0;
    // even number
    bool2 = (n->ob_digit[0] & 1) == 0;
    if (cmp < 0 || cmp == 1 || // occured error or n <= 0
        bool1 || bool2) {
        /*
        printf("cmp = %d, PyLong_Check(n) = %d\n",cmp, PyLong_Check(n));
        printf("(n->ob_digit[0] & 1) = %d\n", (n->ob_digit[0] & 1));
        */
        if (cmp >= 0)
            PyErr_Format(MontgomeryErrorObject, "%S is not appreciate modulus. Modulus must be positive integer and odd number.", self->n);
        Py_XDECREF(self->n);
        Py_XDECREF(zero);
        self->n = NULL;

        return -1;
    }
    Py_DECREF(zero);
    nbit = _bit_length(n) + 1;
    self->nbit = PyLong_FromLong(nbit);
    one = PyLong_FromLong(1);
    two = PyLong_FromLong(2);

    // self.r = 1 << self.nbit
    self->r = PyNumber_Lshift(one, self->nbit);
    // self.r_square = self.r ** 2 % self.n
    self->r_square = PyNumber_Power(self->r, two, self->n);
    Py_DECREF(two);

    rn = Py_BuildValue("OO", self->r, self->n);
    // x, y, gcd = gcdext(self.r, self.n)
    xygcd = montgomery_gcdext(NULL, rn);
    Py_DECREF(rn);
    self->r_ = PySequence_GetItem(xygcd, 0);
    // x, y, gcd = gcdext(self.n, self.r)
    y = PyTuple_GetItem(xygcd, 1);
    // x, y, gcd = gcdext(self.r, self.n)
    x = PyNumber_Subtract(self->r, y);
    Py_DECREF(xygcd);

    self->ffffffff = PyNumber_Subtract(self->r, one);
    Py_DECREF(one);
    // self.n_ = x * (self.r - 1) % self.r
    self->n_ = PyNumber_Multiply(x, self->ffffffff);
    Py_DECREF(x);
    self->n_ = PyNumber_And(self->n_, self->ffffffff);

    self->c_extension = Py_True;

    return 0;
}

static PyObject *
_montgomery_mul(MontgomeryObject *self, PyObject *a, PyObject *b)
{
    // # return a * b * r_ mod n
    PyObject *t, *m, *abr_;
    PyObject *tmp;
    // t = a * b
    t = PyNumber_Multiply(a, b);
    // m = (t & self.ffffffff) * self.n_ & self.ffffffff
    m = PyNumber_And(t, self->ffffffff);
    tmp = PyNumber_Multiply(m, self->n_);
    Py_DECREF(m);
    m = PyNumber_And(tmp, self->ffffffff);
    Py_DECREF(tmp);

    // abr_ = (t + m * self.n) >> self.nbit
    abr_ = PyNumber_Multiply(m, self->n);
    tmp = PyNumber_Add(t, abr_);
    Py_DECREF(abr_);
    abr_ = PyNumber_Rshift(tmp, self->nbit);
    Py_DECREF(tmp);

    // if abr_ > self.n:
    if (PyObject_RichCompareBool(abr_, self->n, Py_GT) == 1) {
    //     abr_ -= self.n
        tmp = abr_;
        abr_ = PyNumber_Subtract(tmp, self->n);
        Py_DECREF(tmp);
    }

    Py_DECREF(m);
    Py_DECREF(t);

    // return abr_
    return abr_;
}

static PyObject *
montgomery_mul(MontgomeryObject *self, PyObject *args)
{
    PyObject *a, *b;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

    // return a * b * self->r_ mod self->n
    return _montgomery_mul(self, a, b);
}

static PyObject *
_montgomery_Fr(MontgomeryObject *self, PyObject *a)
{
    // A = self.mul(a, self.r_square)
    return _montgomery_mul(self, a, self->r_square);
}

static PyObject *
montgomery_Fr(MontgomeryObject *self, PyObject *args)
{
    // return a * self.r mod n
    PyObject *a;

    if (!PyArg_ParseTuple(args, "O", &a))
        return NULL;

    // A = self.mul(a, self.r_square)
    return _montgomery_Fr(self, a);
}

static PyObject *
_montgomery_Fr_inv(MontgomeryObject *self, PyObject *A)
{
    // return A * self.r_ mod n
    PyObject *a, *one;

    // a = self.mul(A, 1)
    one = PyLong_FromLong(1);
    a = _montgomery_mul(self, A, one);
    Py_DECREF(one);

    return a;
}

static PyObject *
montgomery_Fr_inv(MontgomeryObject *self, PyObject *args)
{
    // return A * self.r_ mod n
    PyObject *a, *A, *one;

    if (!PyArg_ParseTuple(args, "O", &A))
        return NULL;

    // a = self.mul(A, 1)
    one = PyLong_FromLong(1);
    args = Py_BuildValue("(OO)", A, one);
    a = montgomery_mul(self, args);
    Py_DECREF(args);
    Py_DECREF(one);

    return a;
}

static PyObject *
_montgomery_pow(MontgomeryObject *self, PyObject *a, PyObject *x)
{
    // return a ^ x mod n
    PyObject *A, *flg, *Y, *y;
    PyObject *one, *tmp, *Lshift;
    int b;
    int count = 0;

    // if not x:
    if (PyObject_Not(x)) {
    //  return 1
        return PyLong_FromLong(1);
    }
    // A = self.Fr(a)
    A = _montgomery_Fr(self, a);

    one = PyLong_FromLong(1);
    // flg = 1 << (x.bit_length() - 1)
    Lshift = PyLong_FromLong(_bit_length((PyLongObject *)x) - 1);
    flg = PyNumber_Lshift(one, Lshift);
    Py_DECREF(Lshift);
    // Y = A
    Y = Py_BuildValue("O", A);
    // flg >>= 1
    tmp = flg;
    flg = PyNumber_Rshift(flg, one);
    Py_DECREF(tmp);
    // while flg:
    while (PyObject_IsTrue(flg)) {
    //  Y = self.mul(Y, Y)
        tmp = Y;
        Y = _montgomery_mul(self, Y, Y);
        Py_DECREF(tmp);
    //  if x & flg:
        tmp = PyNumber_And(x, flg);
        b = PyObject_IsTrue(tmp);
        Py_DECREF(tmp);
        if (b) {
    //      Y = self.mul(Y, A)
            tmp = Y;
            Y = _montgomery_mul(self, Y, A);
            Py_DECREF(tmp);
        }
    //  flg >>= 1
        tmp = flg;
        flg = PyNumber_Rshift(flg, one);
        Py_DECREF(tmp);

        count++;
    }

    // y = self.Fr_inv(Y)
    tmp = Y;
    y = _montgomery_Fr_inv(self, Y);

    Py_DECREF(Y);
    Py_DECREF(A);
    Py_DECREF(flg);
    Py_DECREF(one);

    return y;

    return PyLong_FromLong(1);
}

static PyObject *
montgomery_pow(MontgomeryObject *self, PyObject *args)
{
    PyObject *a, *x;
    if (!PyArg_ParseTuple(args, "OO", &a, &x))
        return NULL;

    return _montgomery_pow(self, a, x);
}

static PyMethodDef Montgomery_methods[] = {
    {"gcdext", montgomery_gcdext, METH_VARARGS, "gcdext()"},
    {"mul", (PyCFunction )montgomery_mul, METH_VARARGS, "mul()"},
    {"Fr", (PyCFunction )montgomery_Fr, METH_VARARGS, "mul()"},
    {"Fr_inv", (PyCFunction )montgomery_Fr_inv, METH_VARARGS, "mul()"},
    {"pow", (PyCFunction )montgomery_pow, METH_VARARGS, "pow()"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef Montgomery_module = {
    PyModuleDef_HEAD_INIT,
    "_montgomery", /* name of module */
    NULL, /* module documentation */
    -1,
    Montgomery_methods
};

static PyTypeObject Montgomery_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_montgomery.Montgomery",             /* tp_name */
    sizeof(MontgomeryObject),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Montgomery_dealloc, /* tp_dealloc */
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
    "Montgomery objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Montgomery_methods,             /* tp_methods */
    Montgomery_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Montgomery_init,      /* tp_init */
    0,                         /* tp_alloc */
    Montgomery_new,                 /* tp_new */
    0,                         /* tp_free Low-level free-memory routine */
};

PyMODINIT_FUNC
PyInit__montgomery(void)
{
    PyObject *m;

    Montgomery_Type.tp_new = Montgomery_new;
    if (PyType_Ready(&Montgomery_Type) < 0)
        return NULL;
    Py_INCREF(&Montgomery_Type);

    m = PyModule_Create(&Montgomery_module);
    if (m == NULL)
        return NULL;
    PyModule_AddObject(m, "Montgomery", (PyObject *)&Montgomery_Type);

    /* Add the Montgomery exception object to the module. */
    MontgomeryErrorObject = PyErr_NewException("_montgomery.MontgomeryError", NULL, NULL);
    if (MontgomeryErrorObject != NULL) {
        Py_INCREF(MontgomeryErrorObject);
        PyModule_AddObject(m, "MontgomeryError", MontgomeryErrorObject);
    }

    PyModule_AddObject(m, "c_extension", Py_True);
    Py_INCREF(Py_True);

    return m;
}
