class C {
	a : Int;
        c : Int;
	init(x : Int) : Int {
           {
		c <- x;
           }
	};
};

class C {
	d : Int;
	init(x : Int) : Int {
           {
		d <- x;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

