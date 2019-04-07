# tun-tap-explore

This is an example C++ project to demonstrate `tun interface`.

## Architecture
![alt text](https://github.com/ddeka0/msp/blob/master/images/compose-architecture.png)

`Postman` gets a response from Team1 server, which contains a list of names of 12 stundets. `Team1` server has `6` names locally. It fetches other `6` names from the `Team2` server.

This is where `Team1` `/students` API is defined.
          

## Instruction to run the project

     sudo apt-get install docker.io
     sudo apt-get install docker-compose
     
     1. git clone https://github.com/ddeka0/msp.git
     2. cd msp
     3. sudo docker-compose up
     4. open postman and try "http://localhost:9091/students"
     
![alt text](https://github.com/ddeka0/msp/blob/master/images/Screenshot%20from%202019-03-11%2023-38-30.png)

## Note regarding docker-compose file
         python folder is for single container explanation with dockerfile
         
         uncomment the volume section in dev mode in dev mode : run npm install inside in the host
         for example in this project
         1. uncomment the volumes sectin
         2. go to team1 and run npm install
         3. go to team2 and run npm install
         4. then to docker-compose up
         
         why we are doing this ? this will help is development mode: auto reload of source files in the host 
         will be reflected inside the container


## Authors

* **Debashish Deka** 
