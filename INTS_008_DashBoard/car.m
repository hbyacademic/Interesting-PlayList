%initialization
real=zeros(1,200);
clock=zeros(1,200);
diff=zeros(1,200);

% Vr/10+4 >= Vd-Vr
% Vr/10+4+Vr >= Vd
% max value of Vd = Vr/10+4+Vr
for n=1:200
  real(n)=n;
  clock(n)=real(n)./10+4+real(n);
  diff(n)=clock(n)-real(n);
end

%plot
x=1:200;
subplot(1,2,1);
plot(x,real,'-r',x,clock,'-');
legend('Vr','Vd');
grid

subplot(1,2,2);
plot(x,diff);
title('Difference between Vr and Vd');
grid


