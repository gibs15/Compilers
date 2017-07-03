class C {
	a : Int;
	init(x : Int) : Int {
           {
		a <- true;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

