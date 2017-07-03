class C {
        a : Int;
	b : Bool;
	init(x : Int,y : Bool) : Bool {
           {
		if y
        	then a <- 3
        	else b <- y 
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

