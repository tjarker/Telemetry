%echoudp("on",3030);
u = udpport("datagram","OutputDatagramSize",16);

write(u,"start","uint8","127.0.0.1",4445);



for i = 1:10
    u.NumDatagramsAvailable
    while u.NumDatagramsAvailable == 0
    end
    data = read(u,u.NumDatagramsAvailable,"uint8");
    d = data.Data
    can.id = bitor(bitshift(d(2),8),d(1),"uint16");
    can.rtr = d(3);
    can.len = d(4);
    can.data64 = d(5);
    for k = 1:7
       can.data64 = bitor(can.data64,bitshift(d(k+5),8*k));
    end
    can.data = d(5:12);
    can.t.hour = d(13);
    can.t.min = d(14);
    can.t.sec = d(15);
    can
    can.t
end
