%r  = 1;
%L  = 2;
%RF = 1;  % Rotational force on the gear
%k  = r/L;


k  = 0.5;
RF = 1;

theta_r_deg = [0:90];
theta_r_rad = pi/180*theta_r_deg;


theta_rad   = asin(k*sin(theta_r_rad));

for i= 1:91
EF(i) = RF*cos(theta_rad(i))/sin(theta_rad(i) + theta_r_rad(i)); % External force on the plunger
end

plot(theta_r_deg,EF);