class C {
	a : Int;
	b : Int <- a;

	init(x : Int, y : Int) : C {
           {
		a <- x;
		b <- y;
		a <- a+b;
		self;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,1)
	};
};
