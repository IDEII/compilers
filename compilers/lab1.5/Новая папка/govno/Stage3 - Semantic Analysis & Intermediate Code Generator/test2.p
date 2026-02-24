program try_it;
const g=0B1011;
type ar = array[1..g] of integer;
	ar1 = array[1..g] of ar;
	re1= record 
			 b:boolean; ch: char; rea:real; c:char 
		end;
	re= record
           x1,x2:real;x6:char; x5:ar1; x3:re1; x4:integer
	   end;
	day = (monday,tuesday,friday);
	se = set of day;
		day_array = array[monday..tuesday] of char;
var x: integer; z,w: real;  r:re; a1:ar1; a: ar; d:day_array; s:se;
procedure kkk;
var z: integer; a:real;
begin
	z := 4+ 8;
	a := z * 32.647
end;
begin
	w := 3.0;
	x := 13;
	z := w * x;
	if (x < 15) then
		x := 50
        else 
            if (x = 15) then
                x := 40
            else x:= 55;

	while x = 5 do
		w := 4.0 ;

	for x := 0 to g do
		w := 5.0;
	a[2] := 4;
	a1[a[2]] := a;
	r.x3.ch := 'v';
	a1[4][a[2]] := a[2];
	{r.x5[4][a[2]] := a[3];}

	with r.x3 do
		begin
			rea := 6.0;
			z := 8.367;
			if (x = 0) then
				x := 1
		end;
	

	x :=5;

	d[monday] := 'c';
	kkk;
	read(x, z, r.x2);

	s := [monday,tuesday,friday];
	write("endofprogram")
	
end.
