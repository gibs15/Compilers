
class C {
	a : Bool;
	b : Bool;
	init(x : Bool, y : Bool) : Bool {
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

