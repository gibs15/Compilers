class C {
	b : Bool;
	init(y : Bool) : Bool {
           {
		b <- y;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

