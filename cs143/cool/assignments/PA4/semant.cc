

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"



#define METHOD_TYPE	5
#define ATTR_TYPE	6


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */

}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 

//
//
// Implementaciones de cool-tree.h

   Symbol class__class::getName(){
      return name;
   }

   Symbol class__class::getParent(){
      return parent;
   }

   Features class__class::getFeatures(){
      return features;
   }

   Symbol class__class::getFilename(){
      return filename;
   }

   void class__class::semant(SymbolTable<Symbol,Symbol>* symtab){
      symtab->enterscope();
      bool hasMain = false;
      cout << getName() << endl;
      for(int i = features->first(); features->more(i); i = features->next(i)){
        Feature feature = features->nth(i);
      	if(name->equal_string("Main",4) != 0 && feature->getFeatureType() == METHOD_TYPE && feature->getName()->equal_string("main",4) != 0){
           hasMain = true;
      	}
        feature->semant(symtab);
      }

      if(!hasMain && getName()->equal_string("Main",4) != 0){
	  cout << "No hay metodo Main" << endl;
      }

      symtab->exitscope();
         
   }



   Symbol method_class::getName(){
      return name;
   }

   Symbol method_class::getTypeDecl(){
      return return_type;
   }

   int method_class::getFeatureType(){
     return featureType;
   }
   //EDITAR
   Symbol method_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      symtab->enterscope();
      expr->semant(symtab);
      symtab->exitscope();
      return return_type;
   }



   Symbol attr_class::getName(){
      return name;
   }

   Symbol attr_class::getTypeDecl(){
      return type_decl;
   }

   int attr_class::getFeatureType(){
     return featureType;
   }
   
   Symbol attr_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(symtab->probe(getName()) == NULL){
         symtab->addid(name,&type_decl);
         if(init->semant(symtab) != type_decl){
            cout << "Se asigno un tipo incompatible a la variable " << name <<  endl;
         }else{
            cout << "TIPOS COMPATIBLES!!!" << endl;
         }
      }else{
         cout << "La variable " << name << " ya esta definida localmente.";
      }

      return type_decl;
   }


   Symbol assign_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      type = expr->semant(symtab);
      if(type != No_type){
         if(symtab->lookup(name) != NULL){
            Symbol declaredType = *(symtab->lookup(name));
            if(declaredType != type){
               cout << "Error, en la asignacion de" << name << ". Tipos incompatibles." << endl;
               type = No_type;
            }
         }else{
            cout << "Error, la variable " << name << " no se encuentra declarada en este scope.";
         }
      }
      return type;
   }



   Symbol cond_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      symtab->enterscope();
      Symbol predType = pred->semant(symtab);
      symtab->exitscope();

      symtab->enterscope();
      Symbol then_expType = then_exp->semant(symtab);
      symtab->exitscope();      

      symtab->enterscope();
      Symbol else_expType = else_exp->semant(symtab);
      symtab->exitscope();

      if(predType != No_type && then_expType != No_type && else_expType != No_type){
         type == No_type;
      }else{
         type = OK_TYPE;
         if(predType != Bool){
            cout << "Error, condicion del if no booleana" << endl;
            type = No_type;
         }

      }

      return type;
   }


   Symbol loop_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      symtab->enterscope();
      Symbol predType = pred->semant(symtab);
      symtab->exitscope();      
  
      symtab->enterscope();
      Symbol bodyType = body->semant(symtab);
      symtab->exitscope();

      if(predType != No_type && bodyType != No_type){
         type == No_type;
      }else{
         type = OK_TYPE;
         if(predType != Bool){
            cout << "Error, condicion del loop no booleana" << endl;
            type = No_type;
         }

      }

      return type;
   }


   Symbol block_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      symtab->enterscope();
      type = OK_TYPE;
      for(int i = body->first(); body->more(i); i = body->next(i)){
        Expression expression = body->nth(i);
        if(expression->semant(symtab) == No_type)
           type = No_type;
      }
      symtab->exitscope();
      return type;
   }


   Symbol plus_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) != No_type || e2->semant(symtab) != No_type){
         Symbol t = Int;
         if(e1->semant(symtab) != Int || e2->semant(symtab) != Int){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }


   Symbol sub_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) != No_type || e2->semant(symtab) != No_type){
         Symbol t = Int;
         if(e1->semant(symtab) != Int || e2->semant(symtab) != Int){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }

   Symbol mul_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) != No_type || e2->semant(symtab) != No_type){
         Symbol t = Int;
         if(e1->semant(symtab) != Int || e2->semant(symtab) != Int){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }

   Symbol divide_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) != No_type || e2->semant(symtab) != No_type){
         Symbol t = Int;
         if(e1->semant(symtab) != Int || e2->semant(symtab) != Int){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }

   Symbol neg_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      type = Int;
      return type;
   }

   Symbol lt_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) != No_type || e2->semant(symtab) != No_type){
         Symbol t = Bool;
         if(e1->semant(symtab) != Int || e2->semant(symtab) != Int){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }

   Symbol eq_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) == No_type || e2->semant(symtab) != No_type){
         Symbol t = e1->semant(symtab);
         if(t != e2->semant(symtab)){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }

   Symbol leq_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      if(e1->semant(symtab) != No_type || e2->semant(symtab) != No_type){
         Symbol t = Bool;
         if(e1->semant(symtab) != Int || e2->semant(symtab) != Int){
            type = No_type;
         }
      }else{
         type = No_type;
      }
      return type;
   }

   Symbol comp_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      type = Bool;
      return type;
   }

   Symbol int_const_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      type = Int;
      return type;
   }

   Symbol bool_const_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      type = Bool;
      return type;
   }

   Symbol string_const_class::semant(SymbolTable<Symbol,Symbol>* symtab){
      type = Str;
      symtab->addid(token,&type);
      return type;
   }

 


/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{

    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */


    
    SymbolTable<Symbol,Symbol> *symtab = new SymbolTable<Symbol, Symbol>();

    symtab->enterscope();

    bool hasMain = false;

    for(int i = classes->first(); classes->more(i); i = classes->next(i)){
        Class_ class_ = classes->nth(i);
	if(symtab->probe(class_->getName()) == NULL){
           symtab->addid(class_->getName(),&Object);
           class_->semant(symtab);
	   if(class_->getName()->equal_string("Main",4) != 0){
              hasMain = true; 
           }
        }else{
           //Error clase repetida
           cout << "Error, la clase " << class_->getName() << " ya fue declarada en este scope." << endl;
        }
    }

    if(hasMain == false){
	cout << "No hay clase Main" << endl;
    }

    symtab->exitscope();

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }

    

    //Separacion para el arbol
    printf("\n\n\n\n\n\n\n");
}


