
class C {
	a : Int;
	b : Int;
	init(x : Int, y : Int) : Int {
           {
		a <- x;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,1)
	};
};

