
class C {
	a : Int;
	b : Bool;
	init(x : Int, y : Bool) : C {
           {
		if (4+8)
        then a <- x
        else b <- y 
        fi;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};


