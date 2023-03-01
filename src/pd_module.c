#include "m_pd.h"
#include "py4pd.h"
#include "py4pd_pic.h"
#include "pd_module.h"

// ======================================
// ======== py4pd embbeded module =======
// ======================================

PyObject *pdout(PyObject *self, PyObject *args){
    (void)self;
    float f;
    char *string;

    // get py4pd object pointer
    // ================================
    PyObject *pd_module = PyImport_ImportModule("__main__");
    PyObject *py4pd_capsule = PyObject_GetAttrString(pd_module, "py4pd");
    t_py *py4pd = (t_py *)PyCapsule_GetPointer(py4pd_capsule, "py4pd");
    // ================================

    if (PyArg_ParseTuple(args, "f", &f)){
        PyErr_Clear();
        outlet_float(py4pd->out_A, f);
    }
    else if (PyArg_ParseTuple(args, "s", &string)){
        t_symbol *pd_symbol = gensym(string);
        outlet_symbol(py4pd->out_A, pd_symbol); // TODO: make this output without the symbol.
        PyErr_Clear();
    }
    else if (PyArg_ParseTuple(args, "O", &args)){
        int list_size = PyList_Size(args);
        t_atom *list_array = (t_atom *)malloc(list_size * sizeof(t_atom));
        int i;
        for (i = 0; i < list_size; ++i)
        {
            PyObject *pValue_i = PyList_GetItem(args, i);
            if (PyLong_Check(pValue_i))
            { // DOC: If the function return a list of integers
                long result = PyLong_AsLong(pValue_i);
                float result_float = (float)result;
                list_array[i].a_type = A_FLOAT;
                list_array[i].a_w.w_float = result_float;
            }
            else if (PyFloat_Check(pValue_i)){ // DOC: If the function return a list of floats
                double result = PyFloat_AsDouble(pValue_i);
                float result_float = (float)result;
                list_array[i].a_type = A_FLOAT;
                list_array[i].a_w.w_float = result_float;
            }
            else if (PyUnicode_Check(pValue_i)){ // DOC: If the function return a list of strings
                const char *result = PyUnicode_AsUTF8(pValue_i);
                list_array[i].a_type = A_SYMBOL;
                list_array[i].a_w.w_symbol = gensym(result);
            }
            else if (Py_IsNone(pValue_i)){   
                // DOC: If the function return a list of None
                // post("None");
            }
            else{
                Py_DECREF(pValue_i);
                Py_DECREF(args);
                return NULL;
            }
        }
        outlet_list(py4pd->out_A, &s_list, list_size, list_array);
        PyErr_Clear();
    }
    else{
        PyErr_SetString(PyExc_TypeError, "[py.script] pd.out argument must be a list, float or a string"); // Colocar melhor descrição do erro
        return NULL;
    }
    return PyLong_FromLong(0);
}


// =================================
PyObject *pdprint(PyObject *self, PyObject *args){
    (void)self;
    char *string;
    if (PyArg_ParseTuple(args, "s", &string)){
        post("[pd.print]: %s", string);
        PyErr_Clear();
    }
    else{
        PyErr_SetString(PyExc_TypeError, "[py.print] must receive a string"); // Colocar melhor descrição do erro
        return NULL;
    }
    return PyLong_FromLong(0);
}

// =================================
PyObject *pderror(PyObject *self, PyObject *args){
    (void)self;
    char *string;
    // get py4pd object pointer
    // ================================
    PyObject *pd_module = PyImport_ImportModule("__main__");
    PyObject *py4pd_capsule = PyObject_GetAttrString(pd_module, "py4pd");
    t_py *py4pd = (t_py *)PyCapsule_GetPointer(py4pd_capsule, "py4pd");
    // ================================

    if (PyArg_ParseTuple(args, "s", &string)){
        pd_error(py4pd, "[py.script]: %s", string);
        PyErr_Clear();
    }
    else{
    PyErr_SetString(PyExc_TypeError, "[py.script] argument of pd.error must be a string"); // Colocar melhor descrição do erro
        return NULL;
    }
    return PyLong_FromLong(0);
} 


// =================================
PyObject *pdsend(PyObject *self, PyObject *args){
    (void)self;
    char* receiver;
    char* string;
    float floatNumber;
    int intNumber;
    PyObject *listargs;

    if (PyArg_ParseTuple(args, "ss", &receiver, &string)){
        t_symbol *symbol = gensym(receiver);
        if (symbol->s_thing) {
            pd_symbol(symbol->s_thing, gensym(string));
        }
        else{
            post("[py.script] pd.send not found object [r %s] in pd patch", receiver);
        }
    }
    else if (PyArg_ParseTuple(args, "sf", &receiver, &floatNumber)){
        t_symbol *symbol = gensym(receiver);
        if (symbol->s_thing) {
            pd_float(symbol->s_thing, floatNumber);
        }
        else{
            post("[py.script] pd.send not found object [r %s] in pd patch", receiver);
        }
    }
    else if (PyArg_ParseTuple(args, "si", &receiver, &intNumber)){
        t_symbol *symbol = gensym(receiver);
        if (symbol->s_thing) {
            pd_float(symbol->s_thing, intNumber);
        }
        else{
            post("[py.script] pd.send not found object [r %s] in pd patch", receiver);
        }
    }
    else if (PyArg_ParseTuple(args, "sO", &receiver, &listargs)){
        if (PyDict_Check(listargs)){
            char error_message[100];
            sprintf(error_message, "[py.send] received a type 'dict', it must be a list, string, int, or float."); 
            PyErr_SetString(PyExc_TypeError, error_message); // TODO: Check english
            return NULL;
        }
        t_atom *list_array;
        int list_size = PyList_Size(listargs);
        list_array = (t_atom *)malloc(list_size * sizeof(t_atom));
        int i;
        for (i = 0; i < list_size; ++i)
        {
            PyObject *pValue_i = PyList_GetItem(listargs, i);
            if (PyLong_Check(pValue_i))
            { // DOC: If the function return a list of integers
                long result = PyLong_AsLong(pValue_i);
                float result_float = (float)result;
                list_array[i].a_type = A_FLOAT;
                list_array[i].a_w.w_float = result_float;
            }
            else if (PyFloat_Check(pValue_i)){ // DOC: If the function return a list of floats
                double result = PyFloat_AsDouble(pValue_i);
                float result_float = (float)result;
                list_array[i].a_type = A_FLOAT;
                list_array[i].a_w.w_float = result_float;
            }
            else if (PyUnicode_Check(pValue_i)){ // DOC: If the function return a list of strings
                const char *result = PyUnicode_AsUTF8(pValue_i);
                list_array[i].a_type = A_SYMBOL;
                list_array[i].a_w.w_symbol = gensym(result);
            }
            else if (Py_IsNone(pValue_i)){   
                // DOC: If the function return a list of None
                // post("None");
            }
            else{
                char error_message[100];
                sprintf(error_message, "[py.send] received a type '%s' in index %d of the list, it must be a string, int, or float.", pValue_i->ob_type->tp_name, i);
                PyErr_SetString(PyExc_TypeError, error_message); // TODO: Check english
                Py_DECREF(pValue_i);
                Py_DECREF(args);
                free(list_array);
                return NULL;
            }
        }
        if (gensym(receiver)->s_thing){
            pd_list(gensym(receiver)->s_thing, &s_list, list_size, list_array);
        }
        else{
            post("[pd.script] object [r %s] not found", receiver);
        }
    }
    else{
        char error_message[100];
        PyObject *pValue_i = PyTuple_GetItem(args, 1);
        sprintf(error_message, "[py.script] pd.send received a type '%s', it must be a string, int, or float.", pValue_i->ob_type->tp_name);
        PyErr_SetString(PyExc_TypeError, error_message); 
        return NULL;
    }
    PyErr_Clear();
    return PyLong_FromLong(0);
}

// =================================
PyObject *pdtabwrite(PyObject *self, PyObject *args, PyObject *keywords){
    (void)self;
    int resize = 0;
    int vecsize;
    t_garray *pdarray;
    t_word *vec;
    char *string;
    PyObject *PYarray;
    
    if (keywords == NULL){
        PyErr_SetString(PyExc_TypeError, "[py.script] pd.tabwrite: keywords must be a dictionary");
        return NULL;
    }
    else{
        resize = PyDict_Contains(keywords, PyUnicode_FromString("resize"));
        if (resize == -1){
            post("error");
        }
        else if (resize == 1){
            PyObject *resize_value = PyDict_GetItemString(keywords, "resize");
            if (resize_value == Py_True){
                resize = 1;
            }
            else if (resize_value == Py_False){
                resize = 0;
            }
            else{
                resize = 0;
            }
        }
        else{
            resize = 0;
        }
    }

    // ================================
    PyObject *pd_module = PyImport_ImportModule("__main__");
    PyObject *py4pd_capsule = PyObject_GetAttrString(pd_module, "py4pd");
    t_py *py4pd = (t_py *)PyCapsule_GetPointer(py4pd_capsule, "py4pd");
    // ================================

    if (PyArg_ParseTuple(args, "sO", &string, &PYarray)){
        t_symbol *pd_symbol = gensym(string);
        if (!(pdarray = (t_garray *)pd_findbyclass(pd_symbol, garray_class)))
            pd_error(py4pd, "[py.script] Array %s not found.", string);
        else if (!garray_getfloatwords(pdarray, &vecsize, &vec))
            pd_error(py4pd, "[py.script] Bad template for tabwrite '%s'.", string);
        else{
            int i;
            if (resize == 1){
                garray_resize_long(pdarray, PyList_Size(PYarray));
                vecsize = PyList_Size(PYarray);
                garray_getfloatwords(pdarray, &vecsize, &vec);
            }
            for (i = 0; i < vecsize; i++){
                double result = PyFloat_AsDouble(PyList_GetItem(PYarray, i));
                float result_float = (float)result;
                vec[i].w_float = result_float;
            }
            garray_redraw(pdarray);
            PyErr_Clear();
        }
    }
    return PyLong_FromLong(0);
} 

// =================================
PyObject *pdtabread(PyObject *self, PyObject *args){
    (void)self;
    int vecsize;
    t_garray *pdarray;
    t_word *vec;
    char *string;

    // ================================
    PyObject *pd_module = PyImport_ImportModule("__main__");
    PyObject *py4pd_capsule = PyObject_GetAttrString(pd_module, "py4pd");
    t_py *py4pd = (t_py *)PyCapsule_GetPointer(py4pd_capsule, "py4pd");
    // ================================

    if (PyArg_ParseTuple(args, "s", &string)){
        t_symbol *pd_symbol = gensym(string);
        if (!(pdarray = (t_garray *)pd_findbyclass(pd_symbol, garray_class))){
            pd_error(py4pd, "[py.script] Array %s not found.", string);
            PyErr_SetString(PyExc_TypeError, "[py.script] pd.tabread: array not found");
        }
        else{
            int i;
            garray_getfloatwords(pdarray, &vecsize, &vec);
            PyObject *list = PyList_New(vecsize);
            for (i = 0; i < vecsize; i++){
                PyList_SetItem(list, i, PyFloat_FromDouble(vec[i].w_float));
            }
            PyErr_Clear();
            return list;

        }
    }
    else{
        PyErr_SetString(PyExc_TypeError, "[py.script] pd.tabread: wrong arguments");
        return NULL;
    }
    return NULL;
}


// =================================
PyObject *pdhome(PyObject *self, PyObject *args){
    (void)self;

    PyObject *pd_module = PyImport_ImportModule("__main__");
    PyObject *py4pd_capsule = PyObject_GetAttrString(pd_module, "py4pd");
    t_py *py4pd = (t_py *)PyCapsule_GetPointer(py4pd_capsule, "py4pd");

    // check if there is no argument
    if (!PyArg_ParseTuple(args, "")){
        PyErr_SetString(PyExc_TypeError, "[py.script] pd.home: no argument expected");
        return NULL;
    }
    return PyUnicode_FromString(py4pd->home_path->s_name);

}

// =================================
PyObject *pdshowimage(PyObject *self, PyObject *args){
    (void)self;
    char *string;
    
    // ================================
    PyObject *pd_module = PyImport_ImportModule("__main__");
    PyObject *py4pd_capsule = PyObject_GetAttrString(pd_module, "py4pd");
    t_py *py4pd = (t_py *)PyCapsule_GetPointer(py4pd_capsule, "py4pd");
    // ================================

    PY4PD_erase(py4pd, py4pd->x_glist);

    if (PyArg_ParseTuple(args, "s", &string)){
        t_symbol *filename = gensym(string);
        if(py4pd->x_def_img){
                py4pd->x_def_img = 0;
        }
        if(glist_isvisible(py4pd->x_glist) && gobj_shouldvis((t_gobj *)py4pd, py4pd->x_glist)){
            const char *file_name_open = PY4PD_filepath(py4pd, filename->s_name);
            if(file_name_open){
                py4pd->x_filename = filename;
                py4pd->x_fullname = gensym(file_name_open);
                if(py4pd->x_def_img){
                        py4pd->x_def_img = 0;
                }
                if(glist_isvisible(py4pd->x_glist) && gobj_shouldvis((t_gobj *)py4pd, py4pd->x_glist)){
                PY4PD_erase(py4pd, py4pd->x_glist);
                sys_vgui("if {[info exists %lx_picname] == 0} {image create photo %lx_picname -file \"%s\"\n set %lx_picname 1\n}\n",
                    py4pd->x_fullname, py4pd->x_fullname, file_name_open, py4pd->x_fullname);
                PY4PD_draw(py4pd, py4pd->x_glist, 0);
                }
            }
            else{
                PyErr_SetString(PyExc_TypeError, "[python]: Error displaying image, file not found");
                return NULL;
            }
        }
        else{
            pd_error(py4pd, "[python]: Error displaying image");
        }

    }
    else{
        PyErr_SetString(PyExc_TypeError, "[py.script] pd.showimage: wrong arguments");
        return NULL;
    }
    return PyLong_FromLong(0);
}


// =================================
PyObject *pdmoduleError;

// =================================
PyMethodDef PdMethods[] = {                                                        
    {"out", pdout, METH_VARARGS, "Output in out0 from PureData"},   
    {"send", pdsend, METH_VARARGS, "Send message to PureData, it can be received with the object [receive]"},
    {"print", pdprint, METH_VARARGS, "Print informations in PureData Console"},            
    {"error", pderror, METH_VARARGS, "Print informations in error format (red) in PureData Console"},
    {"tabwrite", (PyCFunction)pdtabwrite, METH_VARARGS | METH_KEYWORDS, "Write data to PureData tables/arrays"}, 
    {"tabread", pdtabread, METH_VARARGS, "Read data from PureData tables/arrays"},
    {"show", pdshowimage, METH_VARARGS, "Show image in PureData, it must be .gif, .bmp, .ppm"},
    {"home", pdhome, METH_VARARGS, "Get PureData Patch Path Folder"},
    {NULL, NULL, 0, NULL} // 
};

// =================================
struct PyModuleDef pdmodule = {
    PyModuleDef_HEAD_INIT,
    "pd", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    PdMethods, // Methods
    NULL, 
    NULL, 
    NULL, 
    NULL, 
};

// =================================

PyMODINIT_FUNC PyInit_pd(){
    PyObject *m;
    m = PyModule_Create(&pdmodule);
    if (m == NULL){
        return NULL;
    }
    pdmoduleError = PyErr_NewException("spam.error", NULL, NULL); 
    Py_XINCREF(pdmoduleError);
    if (PyModule_AddObject(m, "moduleerror", pdmoduleError) < 0){
        Py_XDECREF(pdmoduleError);
        Py_CLEAR(pdmoduleError);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}
