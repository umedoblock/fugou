#include "Python.h"
#include "structmember.h"

static PyObject *
Py_gcdext(PyObject *self, PyObject *args)
{
    PyObject *a = NULL, *b = NULL;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

    Py_RETURN_NONE;
}

static PyMethodDef gcdext_methods[] = {
    {"gcdext", (PyCFunction )Py_gcdext, METH_VARARGS, "gcdext()"},
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
