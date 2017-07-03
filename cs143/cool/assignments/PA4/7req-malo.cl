class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
        {
		   if(y=true)
		   then    { dd : Bool }
		   else     a <- x
		   fi;   
		   
		   self;
		   
		   dd <- y;
        }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

