int x ;
int main ( )
{
	int a ;
	int b ;
	double c ;
	double d ;
	int e [ 20 ] ;
	int f [ 15 ] ;
	int g ;
	a = 1 ;
	b = 2.2 ;
	c = 3.3 ;
	d = 4 ;
	g = a + c ;
	if( b > a ) {
		a = 5 + a ;
		print a ;
	}
	else {
		b = b + 6 ;
		print b ;
	}
	while ( c < 10) {
		a = a + 1 ;
		c = c + 1 ;
	}
	print a ;
	e [ 0 ] = 1 ;
	f [ 2 ] = 3 ;
	f [ 4 ] = 5 ;
	f [ 4 ] = e [ 0 ] + f [ 2 ] ;
	x = f [ 4 ] ;
	print x ;
	func ( ) ;
	return 0 ; 
}

int func ( )
{
	int test ;
	test = 12345 ;
	print test ;
	return 0 ;
}