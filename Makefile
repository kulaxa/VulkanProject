CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr

VulkanTutorial: *.cpp *.hpp
	g++ $(CFLAGS) -o VulkanTutorial  *.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanTutorial
	./VulkanTutorial

clean:
	rm -f VulkanTutorial

