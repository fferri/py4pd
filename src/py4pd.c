#include "py4pd.h"
#include "module.h"

static int object_count = 1;
static int thread_status[100];
static t_class *py4pd_class; // 


// define global pointer for py_object



// =====================================================================
// ============ Pd Object code =========================================
// =====================================================================

static void home(t_py *x, t_symbol *s, int argc, t_atom *argv) {
    (void)s; // unused but required by pd
    if (argc < 1) {
        post("[py4pd] The home path is: %s", x->home_path->s_name);
        return; 
    } else {
        x->home_path = atom_getsymbol(argv);
        post("[py4pd] The home path set to: %s", x->home_path->s_name);
    }
}

// // ============================================
// // ============================================
// // ============================================

static void packages(t_py *x, t_symbol *s, int argc, t_atom *argv) {
    (void)s; 
    if (argc < 1) {
        post("[py4pd] The packages path is: %s", x->packages_path->s_name);
        return; // is this necessary?
    }
    else {
        if (argc < 2 && argc > 0){
            x->packages_path = atom_getsymbol(argv);
            // probe if the path is valid
            if (access(x->packages_path->s_name, F_OK) != -1) {
                // do nothing
            } else {
                pd_error(x, "The packages path is not valid");
            }
        }   
        else{
            pd_error(x, "It seems that your package folder has |spaces|.");
            return;
        }    
    }
}

// ====================================
// ====================================
// ====================================

static void documentation(t_py *x){
    PyObject *pFunc;
    if (x->function_called == 0) { // if the set method was not called, then we can not run the function :)
        pd_error(x, "[py4pd] To see the documentaion you need to set the function first!");
        return;
    }

    pFunc = x->function;
    if (pFunc && PyCallable_Check(pFunc)){ // Check if the function exists and is callable
        PyObject *pDoc = PyObject_GetAttrString(pFunc, "__doc__"); // Get the documentation of the function
        if (pDoc != NULL){
            const char *Doc = PyUnicode_AsUTF8(pDoc); 
            if (Doc != NULL){
                post("");
                post("[+][+] %s [+][+]", x->function_name->s_name);
                post("");
                post("%s", Doc);
                post("");
            }
            else{

                post("");
                pd_error(x, "[py4pd] No documentation found!");
                post("");
            }
        }
        else{
            post("");
            pd_error(x, "[py4pd] No documentation found!");
            post("");
        }
    }
}

// ====================================
static void py4pd_globalVariables(t_py *x, t_symbol *s, int argc, t_atom *argv){

    (void)s; // unused but required by pd
    // argv[0] = name of the variable
    // argv[1] = value of the variable
    // PyRun_SimpleString("a = 1");

    char *string;
    
    if (argc < 1) {
        pd_error(x, "[py4pd] You need to set the variable name and value");
        return;
    }
    else if (argc < 2) {
        pd_error(x, "[py4pd] You need to set the variable value");
        return;
    }
    else if (argc < 3) {
        string = (char *)malloc(1000);
        sprintf(string, "%s = %s", atom_getsymbol(argv)->s_name, atom_getsymbol(argv+1)->s_name);
        PyRun_SimpleString(string);
        free(string);
    }
    else{
        pd_error(x, "[py4pd] Too many arguments");
        return;
    }
}


// ====================================
// ====================================
// ====================================

void pd4py_system_func (const char *command){
    int result = system(command);
    if (result == -1){
        post("[py4pd] %s", command);
        return;
    }
}

// ============================================
// ============================================

static void create(t_py *x, t_symbol *s, int argc, t_atom *argv){
    // If Windows OS run, if not then warn the user
    (void)s;
    (void)argc;
    
    const char *script_name = argv[0].a_w.w_symbol->s_name;
    post("[py4pd] Opening vscode...");
    #ifdef _WIN64 // ERROR: the endif is missing directive _WIN64

    char *command = malloc(strlen(x->home_path->s_name) + strlen(script_name) + 20);
    sprintf(command, "/c code %s/%s.py", x->home_path->s_name, script_name);
    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    // sei.lpVerb = "open";
    
    sei.lpFile = "cmd.exe ";
    sei.lpParameters = command;
    sei.nShow = SW_HIDE;
    ShellExecuteEx(&sei);
    CloseHandle(sei.hProcess);
    free(command);
    return;

    // Not Windows OS

    #else // if not windows 64bits
    char *command = malloc(strlen(x->home_path->s_name) + strlen(script_name) + 20);
    sprintf(command, "code %s/%s.py", x->home_path->s_name, script_name);

    pd4py_system_func(command);
    return;
    #endif
}

// ====================================
// ====================================
// ====================================

static void vscode(t_py *x){
    // If Windows OS run, if not then warn the user
       
    if (x->function_called == 0) { // if the set method was not called, then we can not run the function :)
        pd_error(x, "[py4pd] To open vscode you need to set the function first!");
        return;
    }
    post("[py4pd] Opening vscode...");
    
    #ifdef _WIN64 // ERROR: the endif is missing directive _WIN64
    char *command = malloc(strlen(x->home_path->s_name) + strlen(x->script_name->s_name) + 20);
    sprintf(command, "/c code %s/%s.py", x->home_path->s_name, x->script_name->s_name);
    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    // sei.lpVerb = "open";
    sei.lpFile = "cmd.exe ";
    sei.lpParameters = command;
    sei.nShow = SW_HIDE;
    ShellExecuteEx(&sei);
    CloseHandle(sei.hProcess);

    return;

    // Not Windows OS

    #else // if not windows 64bits
    char *command = malloc(strlen(x->home_path->s_name) + strlen(x->script_name->s_name) + 20);
    sprintf(command, "code %s/%s.py", x->home_path->s_name, x->script_name->s_name);
    pd4py_system_func(command);
    pd_error(x, "Not tested in your Platform, please send me a bug report!");
    return;
    #endif
}

// ====================================
// ====================================
// ====================================

static void reload(t_py *x){
    PyObject *pName, *pFunc, *pModule, *pReload;
    if (x->function_called == 0) { // if the set method was not called, then we can not run the function :)
        pd_error(x, "To reload the script you need to set the function first!");
        return;
    }
    pFunc = x->function;
    pModule = x->module;

    // reload the module
    pName = PyUnicode_DecodeFSDefault(x->script_name->s_name); // Name of script file
    pModule = PyImport_Import(pName);
    pReload = PyImport_ReloadModule(pModule);
    if (pReload == NULL) {
        pd_error(x, "Error reloading the module!");
        x->function_called = 0;
        Py_DECREF(x->function);
        Py_DECREF(x->module);
        return;
    } else{
        Py_XDECREF(x->module);
        pFunc = PyObject_GetAttrString(pModule, x->function_name->s_name); // Function name inside the script file
        Py_DECREF(pName); // 
        if (pFunc && PyCallable_Check(pFunc)){ // Check if the function exists and is callable 
            x->function = pFunc;
            x->module = pModule;
            x->script_name = x->script_name;
            x->function_name = x->function_name; // why 
            x->function_called = malloc(sizeof(int)); 
            *(x->function_called) = 1; // 
            post("The module was reloaded!");
            return; 
        }
        else{
            pd_error(x, "Error reloading the module!");
            x->function_called = 0;
            Py_DECREF(x->function);
            Py_DECREF(x->module);
            return;
        }
    } 
}

// ====================================
// ====================================
// ====================================

static void set_function(t_py *x, t_symbol *s, int argc, t_atom *argv){
    (void)s;
    t_symbol *script_file_name = atom_gensym(argv+0);
    t_symbol *function_name = atom_gensym(argv+1);

    // Check if the already was set
    if (x->function_name != NULL){
        int function_is_equal = strcmp(function_name->s_name, x->function_name->s_name);
        if (function_is_equal == 0){    // If the user wants to call the same function again! This is not necessary at first glance. 
            pd_error(x, "[py4pd] The function was already called!");
            Py_XDECREF(x->function);
            Py_XDECREF(x->module);
            x->function = NULL;
            x->module = NULL;
            x->function_name = NULL;
        }
        else{ // DOC: If the function is different, then we need to delete the old function and create a new one.
            Py_XDECREF(x->function);
            Py_XDECREF(x->module);
            x->function = NULL;
            x->module = NULL;
            x->function_name = NULL;
        }      
    }
    // Infos
    post("[py4pd] Script file: %s.py", script_file_name->s_name);
    post("[py4pd] Function name: %s", function_name->s_name);
    // Erro handling
    // Check if script has .py extension
    char *extension = strrchr(script_file_name->s_name, '.');
    if (extension != NULL) {
        pd_error(x, "[py4pd] Don't use extensions in the script file name!");
        Py_XDECREF(x->function);
        Py_XDECREF(x->module);
        return;
    }

    // check if script file exists
    char script_file_path[MAXPDSTRING];
    snprintf(script_file_path, MAXPDSTRING, "%s/%s.py", x->home_path->s_name, script_file_name->s_name);
    if (access(script_file_path, F_OK) == -1) {
        pd_error(x, "The script file %s does not exist!", script_file_path);
        Py_XDECREF(x->function);
        Py_XDECREF(x->module);
        return;
    }
    
    // =====================
    // DOC: check number of arguments
    if (argc < 2) { // check is the number of arguments is correct | set "function_script" "function_name"
        pd_error(x,"[py4pd] {set} message needs two arguments! The 'Script name' and the 'function name'!");
        return;
    }
    // =====================
    PyObject *pName, *pModule, *pFunc; // Create the variables of the python objects
    
    // =====================
    // Add aditional path to python to work with Pure Data
    PyObject *home_path = PyUnicode_FromString(x->home_path->s_name);
    PyObject *site_package = PyUnicode_FromString(x->packages_path->s_name);
    PyObject *sys_path = PySys_GetObject("path");
    PyList_Insert(sys_path, 0, home_path);
    PyList_Insert(sys_path, 0, site_package); // BUG: not working
    Py_DECREF(home_path);
    Py_DECREF(site_package);

    // =====================
    pName = PyUnicode_DecodeFSDefault(script_file_name->s_name); // Name of script file
    pModule = PyImport_Import(pName);
    // =====================
    // check if the module was loaded
    if (pModule == NULL) {
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        PyObject *pstr = PyObject_Str(pvalue);
        pd_error(x, "[py4pd] Call failed: %s", PyUnicode_AsUTF8(pstr));
        Py_XDECREF(pstr);
        Py_XDECREF(pModule);
        Py_XDECREF(pName);
        return;
    }
 
    pFunc = PyObject_GetAttrString(pModule, function_name->s_name); // Function name inside the script file
    Py_DECREF(pName); // DOC: Delete the name of the script file
    if (pFunc && PyCallable_Check(pFunc)){ // Check if the function exists and is callable   
        PyObject *inspect=NULL, *getargspec=NULL, *argspec=NULL, *args=NULL;
        inspect = PyImport_ImportModule("inspect");
        getargspec = PyObject_GetAttrString(inspect, "getargspec");
        argspec = PyObject_CallFunctionObjArgs(getargspec, pFunc, NULL);
        args = PyObject_GetAttrString(argspec, "args");
        int py_args = PyObject_Size(args);
        post("[py4pd] Function loaded!");
        post("[py4pd] It has %i arguments!", py_args);
        // x->py_arg_numbers = py_args;
        // warning: assignment to 'int *' from 'int' makes pointer from integer without a cast [-Wint-conversion] in x->py_arg_numbers = py_args;
        // The solution is to use a pointer to the int variable
        x->py_arg_numbers = py_args;
        x->function = pFunc;
        x->module = pModule;
        x->script_name = script_file_name;
        x->function_name = function_name; 
        x->function_called = malloc(sizeof(unsigned int));
        *(x->function_called) = 1; // 
        Py_XDECREF(inspect);
        Py_XDECREF(getargspec);
        Py_XDECREF(argspec);
        Py_XDECREF(args);

    } else {
        // post PyErr_Print() in pd
        pd_error(x, "Function %s not loaded!", function_name->s_name);
        x->function_called = 0; // set the flag to 0 because it crash Pd if user try to use args method
        x->function_name = NULL;
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        PyObject *pstr = PyObject_Str(pvalue);
        pd_error(x, "Call failed:\n %s", PyUnicode_AsUTF8(pstr));
        Py_DECREF(pstr);
        Py_XDECREF(pModule);
        Py_XDECREF(pFunc);
        Py_XDECREF(pName);
        Py_XDECREF(ptype);
        Py_XDECREF(pvalue);
        Py_XDECREF(ptraceback);
    }
}


// ============================================
// ============================================
// ============================================

static void run_function(t_py *x, t_symbol *s, int argc, t_atom *argv){
    (void)s;
    if (argc != x->py_arg_numbers) {
        pd_error(x, "[py4pd] Wrong number of arguments!");
        //pd_error(x, "[py4pd] Function %s has %i arguments!", x->function_name->s_name, *(x->py_arg_numbers));
        
        return;
    }
    PyObject *pFunc, *pArgs, *pValue; // pDict, *pModule,
    pFunc = x->function;
    pArgs = PyTuple_New(argc);
    int i;
    if (x->function_called == 0) { // if the set method was not called, then we can not run the function :)
        if(pFunc != NULL){
            // create t_atom *argv from x->script_name and x->function_name
            t_atom *argv = malloc(sizeof(t_atom) * 2);
            SETSYMBOL(argv, x->script_name);
            SETSYMBOL(argv+1, x->function_name);
            set_function(x, NULL, 2, argv);
        } else{
            pd_error(x, "[py4pd] The message need to be formatted like 'set {script_name} {function_name}'!");
            return;
        }
    }
    
    // DOC: CONVERTION TO PYTHON OBJECTS
    for (i = 0; i < argc; ++i) {
        // NUMBERS 
        if (argv[i].a_type == A_FLOAT) { 
            float arg_float = atom_getfloat(argv+i);
            if (arg_float == (int)arg_float){ // DOC: If the float is an integer, then convert to int
                int arg_int = (int)arg_float;
                pValue = PyLong_FromLong(arg_int);
            }
            else{ // If the int is an integer, then convert to int
                pValue = PyFloat_FromDouble(arg_float);
            }

        // STRINGS
        } else if (argv[i].a_type == A_SYMBOL) {
            pValue = PyUnicode_DecodeFSDefault(argv[i].a_w.w_symbol->s_name); // convert to python string
        } else {
            pValue = Py_None;
            Py_INCREF(Py_None);
        }
        // ERROR IF THE ARGUMENT IS NOT A NUMBER OR A STRING       
        if (!pValue) {
            pd_error(x, "Cannot convert argument\n");
            //PyGILState_Release(gstate);
            return;
        }
        PyTuple_SetItem(pArgs, i, pValue); // DOC: Set the argument in the tuple
    }

    pValue = PyObject_CallObject(pFunc, pArgs);
    if (pValue != NULL) {                                // DOC: if the function returns a value   
        if (PyList_Check(pValue)){                       // DOC: If the function return a list list
            int list_size = PyList_Size(pValue);
            t_atom *list_array = (t_atom *) malloc(list_size * sizeof(t_atom));            
            for (i = 0; i < list_size; ++i) {
                PyObject *pValue_i = PyList_GetItem(pValue, i);
                if (PyLong_Check(pValue_i)) {            // DOC: If the function return a list of integers
                    long result = PyLong_AsLong(pValue_i);
                    float result_float = (float)result;
                    list_array[i].a_type = A_FLOAT;
                    list_array[i].a_w.w_float = result_float;

                } else if (PyFloat_Check(pValue_i)) {    // DOC: If the function return a list of floats
                    double result = PyFloat_AsDouble(pValue_i);
                    float result_float = (float)result;
                    list_array[i].a_type = A_FLOAT;
                    list_array[i].a_w.w_float = result_float;
                } else if (PyUnicode_Check(pValue_i)) {  // DOC: If the function return a list of strings
                    const char *result = PyUnicode_AsUTF8(pValue_i); 
                    list_array[i].a_type = A_SYMBOL;
                    list_array[i].a_w.w_symbol = gensym(result);
                } else if (Py_IsNone(pValue_i)) {        // DOC: If the function return a list of None
                    post("None");
                } else {
                    pd_error(x, "[py4pd] py4pd just convert int, float and string!\n");
                    pd_error(x, "INFO [!] The value received is of type %s", Py_TYPE(pValue_i)->tp_name);
                    Py_DECREF(pValue_i);
                    Py_DECREF(pArgs);
                    return;
                }
            }
            outlet_list(x->out_A, 0, list_size, list_array); // TODO: possible do in other way? Seems slow!
            return;
        } else {
            if (PyLong_Check(pValue)) {
                long result = PyLong_AsLong(pValue); // DOC: If the function return a integer
                outlet_float(x->out_A, result);
                //PyGILState_Release(gstate);
                return;
            } else if (PyFloat_Check(pValue)) {
                double result = PyFloat_AsDouble(pValue); // DOC: If the function return a float
                float result_float = (float)result;
                outlet_float(x->out_A, result_float);
                //PyGILState_Release(gstate);
                return;
                // outlet_float(x->out_A, result);
            } else if (PyUnicode_Check(pValue)) {
                const char *result = PyUnicode_AsUTF8(pValue); // DOC: If the function return a string
                outlet_symbol(x->out_A, gensym(result)); 
                //PyGILState_Release(gstate);
                return;
            // now check if it return None
            } else if (Py_IsNone(pValue)) {
                post("None");
            } else {
                pd_error(x, "[py4pd] py4pd just convert int, float and string!\n");
                pd_error(x, "INFO  [!!!!] The value received is of type %s", Py_TYPE(pValue)->tp_name);
                Py_DECREF(pValue);
                Py_DECREF(pArgs);
                return;
            }
        }
        Py_DECREF(pValue);
    }
    else { // DOC: if the function returns a error
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        PyObject *pstr = PyObject_Str(pvalue);
        pd_error(x, "[py4pd] Call failed: %s", PyUnicode_AsUTF8(pstr));
        Py_DECREF(pstr);
        Py_DECREF(pvalue);
        Py_DECREF(ptype);
        Py_DECREF(ptraceback);
        //PyGILState_Release(gstate);
        return;
    }
}


// ============================================
// ============================================
// ============================================

static void run_function_thread(t_py *x, t_symbol *s, int argc, t_atom *argv){
    // set GIL is held
    PyGILState_STATE gstate;
    gstate = PyGILState_Ensure();
    PyThreadState* ts = PyThreadState_New(x->py_interpreter);
    PyThreadState_Swap(ts);
    
    (void)s;
    if (argc != x->py_arg_numbers) {
        pd_error(x, "[py4pd] Wrong number of arguments!");
        return;
    }
    PyObject *pFunc, *pArgs, *pValue; // pDict, *pModule,
    pFunc = x->function;
    pArgs = PyTuple_New(argc);
    int i;
    if (x->function_called == 0) { // if the set method was not called, then we can not run the function :)
        if(pFunc != NULL){
            // create t_atom *argv from x->script_name and x->function_name
            t_atom *argv = malloc(sizeof(t_atom) * 2);
            SETSYMBOL(argv, x->script_name);
            SETSYMBOL(argv+1, x->function_name);
            set_function(x, NULL, 2, argv);
        } else{
            pd_error(x, "[py4pd] The message need to be formatted like 'set {script_name} {function_name}'!");
            return;
        }
    }
    
    // DOC: CONVERTION TO PYTHON OBJECTS
    for (i = 0; i < argc; ++i) {
        // NUMBERS 
        if (argv[i].a_type == A_FLOAT) { 
            float arg_float = atom_getfloat(argv+i);
            if (arg_float == (int)arg_float){ // DOC: If the float is an integer, then convert to int
                int arg_int = (int)arg_float;
                pValue = PyLong_FromLong(arg_int);
            }
            else{ // If the int is an integer, then convert to int
                pValue = PyFloat_FromDouble(arg_float);
            }

        // STRINGS
        } else if (argv[i].a_type == A_SYMBOL) {
            pValue = PyUnicode_DecodeFSDefault(argv[i].a_w.w_symbol->s_name); // convert to python string
        } else {
            pValue = Py_None;
            Py_INCREF(Py_None);
        }
        // ERROR IF THE ARGUMENT IS NOT A NUMBER OR A STRING       
        if (!pValue) {
            pd_error(x, "Cannot convert argument\n");
            return;
        }
        PyTuple_SetItem(pArgs, i, pValue); // DOC: Set the argument in the tuple
    }


    pValue = PyObject_CallObject(pFunc, pArgs); // DOC: Call the function


    if (pValue != NULL) {                                // DOC: if the function returns a value   
        if (PyList_Check(pValue)){                       // DOC: If the function return a list list
            int list_size = PyList_Size(pValue);
            t_atom *list_array = (t_atom *) malloc(list_size * sizeof(t_atom));            
            for (i = 0; i < list_size; ++i) {
                PyObject *pValue_i = PyList_GetItem(pValue, i);
                if (PyLong_Check(pValue_i)) {            // DOC: If the function return a list of integers
                    long result = PyLong_AsLong(pValue_i);
                    float result_float = (float)result;
                    list_array[i].a_type = A_FLOAT;
                    list_array[i].a_w.w_float = result_float;

                } else if (PyFloat_Check(pValue_i)) {    // DOC: If the function return a list of floats
                    double result = PyFloat_AsDouble(pValue_i);
                    float result_float = (float)result;
                    list_array[i].a_type = A_FLOAT;
                    list_array[i].a_w.w_float = result_float;
                } else if (PyUnicode_Check(pValue_i)) {  // DOC: If the function return a list of strings
                    const char *result = PyUnicode_AsUTF8(pValue_i); 
                    list_array[i].a_type = A_SYMBOL;
                    list_array[i].a_w.w_symbol = gensym(result);
                } else if (Py_IsNone(pValue_i)) {        // DOC: If the function return a list of None
                     // post("None");
                
                } else {
                    pd_error(x, "[py4pd] py4pd just convert int, float and string!\n");
                    pd_error(x, "INFO  [!] The value received is of type %s", Py_TYPE(pValue_i)->tp_name);
                    Py_DECREF(pValue_i);
                    Py_DECREF(pArgs);                   
                }
            }
            // release the GIL
            // PyGILState_Release(gstate);
            PyThreadState *_save = NULL;
            PyEval_RestoreThread(_save);
            
            outlet_list(x->out_A, 0, list_size, list_array); // TODO: possible do in other way? Seems slow!
            
        } else {
            if (PyLong_Check(pValue)) {
                long result = PyLong_AsLong(pValue); // DOC: If the function return a integer
                // PyGILState_Release(gstate);
                outlet_float(x->out_A, result);
                
            } else if (PyFloat_Check(pValue)) {
                double result = PyFloat_AsDouble(pValue); // DOC: If the function return a float
                float result_float = (float)result;
                // PyGILState_Release(gstate);
                outlet_float(x->out_A, result_float);
                
            } else if (PyUnicode_Check(pValue)) {
                const char *result = PyUnicode_AsUTF8(pValue); // DOC: If the function return a string
                outlet_symbol(x->out_A, gensym(result)); 
                
            // now check if it return None
            } else if (Py_IsNone(pValue)) {
                post("None");
                // PyGILState_Release(gstate);
                //post("Returning");
                return;
            } else {
                pd_error(x, "[py4pd] py4pd just convert int, float and string!\n");
                pd_error(x, "INFO  [!!!] The value received is of type %s", Py_TYPE(pValue)->tp_name);
                Py_DECREF(pValue);
                Py_DECREF(pArgs);
                
            }
        }
        Py_DECREF(pValue);
    }
    else { // DOC: if the function returns a error
        PyObject *ptype, *pvalue, *ptraceback;
        PyErr_Fetch(&ptype, &pvalue, &ptraceback);
        PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);
        PyObject *pstr = PyObject_Str(pvalue);
        pd_error(x, "[py4pd] Call failed: %s", PyUnicode_AsUTF8(pstr));
        Py_DECREF(pstr);
        Py_DECREF(pvalue);
        Py_DECREF(ptype);
        Py_DECREF(ptraceback);
    }
    Py_DECREF(pArgs);
    PyThreadState_Clear(ts);
    PyThreadState_DeleteCurrent();
    PyEval_RestoreThread(ts);
    return;
}

// ============================================
// ============================================
// ============================================
struct thread_arg_struct {
    t_py x;
    t_symbol s;
    int argc;
    t_atom *argv;
    PyGILState_STATE gil_state;
} thread_arg;

// ============================================
// ============================================
// ============================================

static void *ThreadFunc(void *lpParameter) {
    struct thread_arg_struct *arg = (struct thread_arg_struct *)lpParameter;
    t_py *x = &arg->x; 
    t_symbol *s = &arg->s;
    int argc = arg->argc;
    t_atom *argv = arg->argv;
    int object_number = *(x->object_number);
    thread_status[object_number] = 1;
    
    run_function_thread(x, s, argc, argv);

    thread_status[object_number] = 0;
    return NULL;
}

// create_thread in Linux
static void create_thread(t_py *x, t_symbol *s, int argc, t_atom *argv){
    (void)s;
    struct thread_arg_struct *arg = (struct thread_arg_struct *)malloc(sizeof(struct thread_arg_struct));
    arg->x = *x;
    arg->argc = argc;
    arg->argv = argv;
    int object_number = *(x->object_number);
    if (x->function_called == 0) {
        // Pd is crashing when I try to create a thread.
        pd_error(x, "[py4pd] You need to call a function before run!");
        free(arg);
        return;
    } else {
        if (thread_status[object_number] == 0){
            pthread_t thread;
            pthread_t hThread;
            hThread = pthread_create(&thread, NULL, ThreadFunc, arg);
            // convert int * to int
            int state = 1;
            x->state = &state;
            // check the Thread was created
            if (hThread != 0) {
                pd_error(x, "[py4pd] CreateThread failed");
                free(arg);
                return;
            } else {
                return;
            }
        } else {
            pd_error(x, "[py4pd] There is a thread running in this Object!");
            free(arg);
            return;
        }
    }
}

// ============================================
static void run(t_py *x, t_symbol *s, int argc, t_atom *argv){
    // convert pointer x->thread to a int
    
    int thread = *(x->thread);
    if (thread == 1) {
        // create new python interpreter
        create_thread(x, s, argc, argv);

    } else if (thread == 2) {
        run_function(x, s, argc, argv);
    } else {
        pd_error(x, "[py4pd] Thread not created");
    }
}

// ============================================
// ============================================
// ============================================

static void thread(t_py *x, t_floatarg f){
    int thread = (int)f;
    if (thread == 1) {
        post("[py4pd] Threading enabled");
        x->thread = malloc(sizeof(int)); 
        *(x->thread) = 1; // 
        // release the GIL
        PyGILState_STATE gstate;
        gstate = PyGILState_Ensure();
        return;
    } else if (thread == 0) {
        x->thread = malloc(sizeof(int)); 
        *(x->thread) = 2; // 
        post("[py4pd] Threading disabled");
        return;
    } else {
        pd_error(x, "[py4pd] Threading status must be 0 or 1");
    }
}

// ============================================
// =========== SETUP OF OBJECT ================
// ============================================

void *py4pd_new(t_symbol *s, int argc, t_atom *argv){ 


    t_py *x = (t_py *)pd_new(py4pd_class);
    // credits
    post("");
    post("[py4pd] py4pd by Charles K. Neimog");
    post("[py4pd] version 0.0.3       ");
    post("[py4pd] based on Python 3.10.5  ");
    post("[py4pd] inspired by the work of Thomas Grill and SOPI research group.");
    // Object count
    x->object_number = malloc(sizeof(int));
    *(x->object_number) = object_count;
    object_count++;
    x->out_A = outlet_new(&x->x_obj, 0); // cria um outlet 
    x->x_canvas = canvas_getcurrent(); // pega o canvas atual
    t_canvas *c = x->x_canvas;  // p
    char *patch_dir = canvas_getdir(c); // directory of opened patch

    PyImport_AppendInittab("pd", PyInit_pd); // DOC: Add the pd module to the python interpreter
    const wchar_t *py_name_ptr; // 
    char *program_name = malloc(sizeof(char) * 10); // 
    sprintf(program_name, "py4pd_%d", *(x->object_number)); // 
    py_name_ptr = Py_DecodeLocale(program_name, NULL); // 
    Py_SetProgramName(py_name_ptr); //
    // check if python is initialized
    
    if (!Py_IsInitialized()) {
        Py_Initialize(); // initialize python
        // PyEval_SaveThread();
    }

    // get  PyInterpreterState 
    PyInterpreterState *interp = PyInterpreterState_Head();
    x->py_interpreter = interp;
    
    x->home_path = patch_dir;     // set name of the home path
    x->packages_path = patch_dir; // set name of the packages path
    x->thread = malloc(sizeof(int));   // set thread status
    *(x->thread) = 2; // solution but it is weird!
    post("[py4pd] Home folder is: %s", x->home_path->s_name);
    
    // check if in x->home_path there is a file py4pd.config
    char *config_path = (char *)malloc(sizeof(char) * (strlen(x->home_path->s_name) + strlen("/py4pd.cfg") + 1)); // 
    strcpy(config_path, x->home_path->s_name); // copy string one into the result.
    strcat(config_path, "/py4pd.cfg"); // append string two to the result.
    if (access(config_path, F_OK) != -1) { // check if file exists
        FILE* file = fopen(config_path, "r"); /* should check the result */
        char line[256]; // line buffer
        while (fgets(line, sizeof(line), file)) { // read a line
            if (strstr(line, "packages =") != NULL) { // check if line contains "packages ="
                char *packages_path = (char *)malloc(sizeof(char) * (strlen(line) - strlen("packages = ") + 1)); // 
                strcpy(packages_path, line + strlen("packages = ")); // copy string one into the result.
                packages_path[strlen(packages_path) - 1] = '\0'; // remove last character
                if (strlen(packages_path) > 0) { // check if path is not empty
                    x->packages_path = gensym(packages_path); // set name
                    post("[py4pd] Packages path set to %s", packages_path); // print path
                }
                free(packages_path); // free memory
            }
        }
        fclose(file); // close file
        // free(line); // free memory
    } else {
        post("[py4pd] Could not find py4pd.cfg in home directory"); // print path
    }
    free(config_path); // free memory
    if (argc > 1) { // check if there are two arguments
        set_function(x, s, argc, argv); // this not work with python submodules
    }
    // save the t_py *x in a global variable TODO: How send this using PyCObject?
    py4pd_object = x;
    return(x);
}

// ============================================
// =========== REMOVE OBJECT ==================
// ============================================

void py4pd_free(t_py *x){
    PyObject  *pModule, *pFunc; // pDict, *pName,
    pFunc = x->function;
    pModule = x->module;
    object_count--;
    // clear all struct
    if (pModule != NULL) {
        Py_DECREF(pModule);
    }
    if (pFunc != NULL) {
        Py_DECREF(pFunc);
    }
}

// ====================================================
void py4pd_setup(void){
    py4pd_class =     class_new(gensym("py4pd"), // cria o objeto quando escrevemos py4pd
                        (t_newmethod)py4pd_new, // metodo de criação do objeto             
                        (t_method)py4pd_free, // quando voce deleta o objeto
                        sizeof(t_py), // quanta memoria precisamos para esse objeto
                        CLASS_DEFAULT, // nao há uma GUI especial para esse objeto???
                        A_GIMME, // o argumento é um símbolo
                        0); // todos os outros argumentos por exemplo um numero seria A_DEFFLOAT
    
    // add method for bang
    class_addbang(py4pd_class, run);

    // Iterate with object
    class_addmethod(py4pd_class, (t_method)vscode, gensym("click"), 0, 0); // when click open vscode

    // Config
    class_addmethod(py4pd_class, (t_method)home, gensym("home"), A_GIMME, 0); // set home path
    class_addmethod(py4pd_class, (t_method)packages, gensym("packages"), A_GIMME, 0); // set packages path
    class_addmethod(py4pd_class, (t_method)set_function, gensym("set"), A_GIMME, 0); // set function to be called
    class_addmethod(py4pd_class, (t_method)run, gensym("run"), A_GIMME, 0);  // run function
    class_addmethod(py4pd_class, (t_method)thread, gensym("thread"), A_FLOAT, 0); // on/off threading
    class_addmethod(py4pd_class, (t_method)py4pd_globalVariables, gensym("global"), A_GIMME, 0); // on/off debug
    class_addmethod(py4pd_class, (t_method)vscode, gensym("vscode"), 0, 0); // open vscode
    class_addmethod(py4pd_class, (t_method)reload, gensym("reload"), 0, 0); // reload python script
    class_addmethod(py4pd_class, (t_method)create, gensym("create"), A_GIMME, 0); // create file or open it

    // Documentation
    class_addmethod(py4pd_class, (t_method)documentation, gensym("doc"), 0, 0); // open documentation

    }

// ==================== PD FUNCTIONS INSIDE PYTHON ====================
// ====================================================================
// ====================================================================

// // dll export function
#ifdef _WIN64

__declspec(dllexport) void py4pd_setup(void); // when I add python module for some reson pd not see py4pd_setup

#endif

