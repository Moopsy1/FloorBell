# FloorBell [work in progress]

This is a personal project using arduino micro controllers.

Scenario:
During my time working at a clothing manufacturing facility I observed that the floor staff had no way of contacting the service departments, besides holding up their hand for the assistant of the production line. The production line assistant would then walk to the relevant service delivery person to bring the service person to the production staff member.

Sollution:
This project aims to solve the problem of unnecesary movement by using arduino microcontrollers with HC12 wireless transcievers.
The floorstaff would be able to ring for assistance from a service department.

Workflow of system:
1 - button is pressed on Arduino 

2 - request is sent over serial on HC12 detailing the request

  2.1 this request is repeated until an ACK is recieved
  
3 - Base station recieved request and returns ACK,NODE_ID

4 - Base station sends request to open ticket to application

  4.1 application opens ticket if NODE_ID is present in database
  
5 - service member attends to callout 

6 - once service member returns to their base station the ticket is closed on the GUI and time_to_close is recorded


Tech:
Qt for GUI

C++ for logic

SQLite for Db

Arduino Nano for serial comms

HC12 for wireless coms

