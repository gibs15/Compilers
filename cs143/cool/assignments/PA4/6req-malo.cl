class C {
        a : Int;
	b : Bool;
	init(x : Int,y : Bool) : Bool {
           {
		if y
        	then a <- true
        	else b <- 0
        	fi;
                true;
           }
	};
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};

