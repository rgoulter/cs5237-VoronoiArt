#ifndef LIH
#define LIH

class LongInt {

	protected:
		void setZero_(); // set this number to be zero

	public:
		LongInt();
		LongInt(LongInt&); // copy constructor
		LongInt(int);      // take in an integer 
		void dump();       // print out this number

		LongInt& operator=(int); 
		LongInt& operator=(LongInt&);  
		LongInt operator+(LongInt&); 
		LongInt operator+(int i) {return (*this) + LongInt(i);};
		LongInt operator-(LongInt&);
		LongInt operator-(int i) {return (*this) + LongInt(i);};
		LongInt operator*(LongInt&);
		LongInt operator*(int i) {return (*this) * LongInt(i);};
		
		bool operator>(LongInt&);
		bool operator<(LongInt&);
		bool operator==(LongInt&);

		bool eqZero();  // return true if it is zero
		int sign();     // return +1 if it's positive, -1 if it's negative, 0 if it's zero


		double doubleValue(); // return a double approximation (you can assume that it will not be out of bound)

	friend LongInt operator-(LongInt&); 
};

LongInt operator-(LongInt&); // negative sign


#endif