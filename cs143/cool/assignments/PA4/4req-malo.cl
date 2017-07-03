class C {
	b : Bool;
	init(y : Bool) : Bool {
           {
		b <- 56;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

