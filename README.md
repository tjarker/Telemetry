# 31015 Fagprojekt - ROAST Telemetry


|<div style="width:200px">Overleaf</div>|<div style="width:200px">Trello</div>|<div style="width:200px">BitBucket</div>|<div style="width:200px">Jira</div>|
|:-:|:-:|:-:|:-:|
|[<img src="https://cdn.overleaf.com/img/ol-brand/overleaf_og_logo.png" width="50">](https://www.overleaf.com/project/602635b948e4260c4d50d582)|[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcQJHNFZpUwJriI-BJL8yI4ND9OfW6uAEWvz0A&usqp=CAU" width="50">](https://trello.com/31015fagprojektelektroteknologigroup7/home)|[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTDNgciuROD6Bc5aQ3lTapXG5fSUVKS6mcZlQ&usqp=CAU" width="50">](https://bitbucket.org/dtucar/)|[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSzVSXn5Ra-QlTUKpbRqWyiRDBx1aJV0lKfwg&usqp=CAU" width="50">](https://jira.dtucar.com/secure/Dashboard.jspa)|
|**Literature & Links**|**Components & Software**|**EcoCar Wiki**|**Shopping List**|
|[<img src="https://static.thenounproject.com/png/251053-200.png" width="50">](documentation/literature.md)|[<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSJIyUJLYjAW1EF-5cv5lt_mT8VVFh0rgjwmA&usqp=CAU" width="50">](documentation/components.md)|[<img src="documentation/resources/wikipedia_PNG35.png" width="50">](https://dtucar.com/wiki/index.php?title=Main_Page)|[<img src="https://image.flaticon.com/icons/png/512/263/263142.png" width="50">](documentation/shoppingList.md)
|**Implementations**|**Specifications**|**Code**||
|[<img src="https://static.thenounproject.com/png/712681-200.png" width="50">](documentation/implementations.md)|[<img src="https://image.flaticon.com/icons/png/512/1541/1541514.png" width="50">](documentation/specification.md)|[<img src="https://cdn2.iconfinder.com/data/icons/font-awesome/1792/code-512.png" width="50">](code)
---

# Documentation

- [Solar Car Unit](documentation/SolarCarUnit.md)
- [Support Vehicle Unit](documentation/supportVehicleUnit.md)
- [RF Transmission Protocol](documentation/transmissionProtocol.md)
- [SCU Commands](documentation/scuCommands.md)

# Open Issue List
- Which Frequencies are open for public use in Australia?
- Do we need a new CAN library? The one linked to is for Teensy 4.0.
- Do we want to use a real time OS? If so, there is no freeRTOS port for the Teensy 3.6. [ChibiOS](https://platformio.org/lib/show/2986/ChRt) seems like a nice alternative.
- A protocol for wireless data transmission needs to be specified (see [RF Transmission Protocol](documentation/transmissionProtocol.md)). 
  - Do the RF modules include some kind of error correction/detection by default? Or should we add our own (i.e. parity or CRC?)?
  - Do we need to worry about security? Should we integrate some kin dof authentication method to secure communication between SV and SC? Exposing the CAN bus itself on an open to use frequency seems risky otherwise.
  - How are messages framed/terminated? Fixed length, length specified in message header or something else?
- How do we wish to implement the final Solar Car and Support Car modules? 
  - PCB design [(just to solder on Teensy)](https://oshpark.com/shared_projects/eowWCxeK)
  - Perfboard 
  - Stripboard 
  - Custom

# Meeting Notes

<table>
<tr><th style='text-align:center'>
February 
</th><th style='text-align:center'> 
March
</th><th style='text-align:center'>
April
</th><th style='text-align:center'>
May
</th><th style='text-align:center'>
June
</th></tr style='text-align:center'>
<tr><td>

- [Meeting 12.02.](documentation/meetingnotes/meeting12_02.md)

- [Meeting 15.02.](documentation/meetingnotes/meeting15_02.md)

- [Meeting 19.02.](documentation/meetingnotes/meeting19_02.md)

- [Meeting 22.02.](documentation/meetingnotes/meeting22_02.md)

- [Meeting 26.02. Group](documentation/meetingnotes/meeting26_02group.md)

- [Meeting 26.02. ROAST](documentation/meetingnotes/meeting26_02roast.md)

- [Meeting 26.02. Martin](documentation/meetingnotes/meeting26_02martin.md)

</td><td>
  
- [Meeting 05.03. ROAST](documentation/meetingnotes/meeting05_03roast.md)

- [Meeting 05.03. Martin](documentation/meetingnotes/meeting05_03martin.md)

- [Meeting 12.03. ROAST](documentation/meetingnotes/meeting12_03roast.md)

- [Meeting 19.03. ROAST](documentation/meetingnotes/meeting19_03roast.md)

- [Meeting 19.03. Martin](documentation/meetingnotes/meeting19_03martin.md)

- [Meeting 26.03. ROAST](documentation/meetingnotes/meeting26_03roast.md)

</td><td>

</td><td>

</td><td>

</td></tr> </table>
