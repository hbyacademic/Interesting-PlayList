%{
x=0:0.01:1;
plot(x,x,'b-',x,x.^(1.5),'r-',x,x.^(2.2),'g-',x,x.^(3),'y-');
legend('gamma 2.2','gamma 1.5','gamma 1','gamma 3');
%}
x=0:0.01:1;
subplot(2,2,1);
plot(x,x,'k--',x,x.^(2.2),'b-');
title('compensation: gamma 1');
subplot(2,2,2);
plot(x,x,'k--',x,x,'b-',x,x.^(2.2),'r-',x,x.^(1/2.2),'g-');
title('compensation: gamma 1/2.2');
subplot(2,2,3);
plot(x,x,'k--',x,x.^(2.2).^(1/1.5),'b-',x,x.^(2.2),'r-',x,x.^(1/1.5),'g-');
title('compensation: gamma 1/1.5');
subplot(2,2,4);
plot(x,x,'k--',x,x.^(2.2).^(1/3),'b-',x,x.^(2.2),'r-',x,x.^(1/3),'g-');
title('compensation: gamma 1/3');
%{
% syms -> symbolic variables
syms x z
z=0:0.1:5;

%function
f=x.^(z-1)*exp(-x);

% integral, from 0 to infinity
int(f,x,0,inf);

% plot(z,f(z))
plot(z,int(f,x,0,inf));

%compare to factorial function
%f(z)=(z-1)!
z2=1:5;
plot(z,int(f,x,0,inf),'-',z2,factorial(z2-1),'*');

%tile, using 'latex' interpreter
%\Gamma -> gamma symbol
%\int -> integral symbol
%\infty -> infinity
% _ -> subscript
% ^ -> superscript
title('$\Gamma(z) = \int _0 ^\infty x^{(z-1)}e^{(-x)}dx$','Interpreter','latex')
grid on;

%put the names in graph
legend('gamma','factorial');
%}