CXX=g++
CXXFLAGS= -std=c++11 -g

EXECUTABLE=smart_cctv

$(EXECUTABLE): main.o cctv_daemon_apis.o camera_daemon.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cpp cctv_daemon_apis.h camera_daemon.h
	$(CXX) $(CXXFLAGS) -c $<

cctv_daemon_apis.o: cctv_daemon_apis.cpp cctv_daemon_apis.h camera_daemon.h
	$(CXX) $(CXXFLAGS) -c $<

camera_daemon.o: camera_daemon.cpp camera_daemon.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf $(EXECUTABLE) *.o
