class C {
	a : Int;
	init(x : Int) : Int {
           {
		c <- x;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

