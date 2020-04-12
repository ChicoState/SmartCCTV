CXX=g++
CXXFLAGS= -std=c++11 -g

EXECUTABLE=smart_cctv

$(EXECUTABLE): main.o high_level_cctv_daemon_apis.o low_level_cctv_daemon_apis.o camera_daemon.o
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cpp high_level_cctv_daemon_apis.h
	$(CXX) $(CXXFLAGS) -c $<

high_level_cctv_daemon_apis.o: high_level_cctv_daemon_apis.cpp high_level_cctv_daemon_apis.h low_level_cctv_daemon_apis.h
	$(CXX) $(CXXFLAGS) -c $<

low_level_cctv_daemon_apis.o: low_level_cctv_daemon_apis.cpp low_level_cctv_daemon_apis.h camera_daemon.h
	$(CXX) $(CXXFLAGS) -c $<

camera_daemon.o: camera_daemon.cpp camera_daemon.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf $(EXECUTABLE) *.o
