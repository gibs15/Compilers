
class C {
	a : Int;
	b : Int;
	init(x : Int, y : Int) : C {
           {
		if y = x
        then a <- x
        else b <- y
        fi;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,1)
	};
};

