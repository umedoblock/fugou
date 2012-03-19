#include "Python.h"
#include "structmember.h"

/* gcdext(a, b) return (gcd, x, y) that satisfy gcd = a * x - b * y */
static PyObject *
Py_gcdext(PyObject *self, PyObject *args)
{
    PyObject *a = NULL, *b = NULL;
    PyObject *tup = Py_None, *gcd = Py_None, *x = Py_None, *y = Py_None;
    PyObject *x1 = Py_None, *y1 = Py_None, *z1 = Py_None;
    PyObject *x2 = Py_None, *y2 = Py_None, *z2 = Py_None;
    PyObject *zero = Py_None, *one = Py_None, *tmp = Py_None;
    PyObject *div = Py_None, *div_ = Py_None, *div_z2 = Py_None;
    int ret = -1;

#ifdef Py_REF_DEBUG
fprintf(stderr, "_Py_RefTotal = %d gcdext() start.\n", _Py_GetRefTotal());
#endif
    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

    zero = Py_BuildValue("i", 0);
    one = Py_BuildValue("i", 1);

/*
    x, y, gcd  = 1, 0, a
    x1, y1, z1 = 0, 1, b
*/
    /* x = one, y = zero, gcd = a; */
    x = Py_BuildValue("i", 1);
    y = Py_BuildValue("i", 0);
    gcd = Py_BuildValue("O", a);

    /* x1 = zero, y1 = one, z1 = b; */
    x1 = Py_BuildValue("i", 0);
    y1 = Py_BuildValue("i", 1);
    z1 = Py_BuildValue("O", b);

/*
    while z1 > 0:
*/
    while ((ret = PyObject_RichCompareBool(z1, zero, Py_GT)) == 1) {
    /*  div, z2 = divmod(gcd, z1) */
        div_z2 = PyNumber_Divmod(gcd, z1);
        div = PySequence_GetItem(div_z2, 0);
        z2 = PySequence_GetItem(div_z2, 1);

        Py_DECREF(div_z2);

    /*  div_ = div * x1 */
        div_ = PyNumber_Multiply(div, x1);
    /*  x2 = x - div_ */
        x2 = PyNumber_Subtract(x, div_);
        Py_DECREF(div_);

    /*  div_ = div * y1 */
        div_ = PyNumber_Multiply(div, y1);
    /*  y2 = y - div_ */
        y2 = PyNumber_Subtract(y, div_);
        Py_DECREF(div_);

        Py_DECREF(div);

    /*  x, y, gcd = x1, y1, z1 */
        Py_DECREF(x); Py_DECREF(y); Py_DECREF(gcd);
        x = x1, y = y1, gcd = z1;
    /*  x1, y1, z1 = x2, y2, z2 */
        x1 = x2, y1 = y2, z1 = z2;
    }
    if (ret < 0)
        goto err;

/*  # print(gcd, x, y) */
/*  if y < 0: */
    if ((ret = PyObject_RichCompareBool(y, zero, Py_LT)) == 1) {
/*      y = -y */
        tmp = y;
        y = PyNumber_Negative(y);
        Py_DECREF(tmp);
    }
    if (ret < 0)
        goto err;

/*  if x < 0: */
    if ((ret = PyObject_RichCompareBool(x, zero, Py_LT)) == 1) {
/*      x += b */
        tmp = x;
        x = PyNumber_Add(x, b);
        Py_DECREF(tmp);

/*      y = a - y */
        tmp = y;
        y = PyNumber_Subtract(a, y);
        Py_DECREF(tmp);
    }
    if (ret < 0)
        goto err;

err:
    if (ret < 0) {
        PyErr_Print();
        tup = NULL;
    }
    else {
        tup = PyTuple_Pack(3, gcd, x, y);
        Py_DECREF(gcd);
        Py_DECREF(x);
        Py_DECREF(y);
    }

    Py_DECREF(x1);
    Py_DECREF(y1);
    Py_DECREF(z1);
    Py_DECREF(one);
    Py_DECREF(zero);

#ifdef Py_REF_DEBUG
fprintf(stderr, "_Py_RefTotal = %d gcdext() done.\n", _Py_GetRefTotal());
#endif
    return tup;
}

static PyMethodDef gcdext_methods[] = {
    {"gcdext", (PyCFunction )Py_gcdext, METH_VARARGS,
        "gcdext(a, b) return (gcd, x, y) that satisfy gcd = a * x - b * y"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef gcdext_module = {
    PyModuleDef_HEAD_INIT,
    "_gcdext", /* name of module */
    NULL, /* module documentation */
    -1,
    gcdext_methods
};

PyMODINIT_FUNC
PyInit__gcdext(void)
{
    /* PyModuleObject *m; */
    PyObject *m;

    m = (PyObject *)PyModule_Create(&gcdext_module);
    if (m == NULL) {
        return NULL;
    }

    return m;
}

PyMODINIT_FUNC
PyInit__gcdext_pg(void)
{
    return PyInit__gcdext();
}
