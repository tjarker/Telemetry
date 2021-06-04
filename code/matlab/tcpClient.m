t = tcpclient('127.0.0.1', 6666); 

for i = 1:10
read(t)
end 
clear t 