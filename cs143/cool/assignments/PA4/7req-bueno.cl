class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
        {
		   if(y=true)
		   then     a <- x
		   else     b <- y
		   fi;    
		   
		   self;
		   
        }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

