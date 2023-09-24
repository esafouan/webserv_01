SRC =  multiplexer.cpp  socket.cpp response_helper.cpp \
		Request/FillHeaders.cpp Request/Request.cpp Request/RequestErrors.cpp Request/HandleUri.cpp Request/RequestUtils.cpp Request/PostElements.cpp Request/Delete.cpp\
		config_file/location.cpp config_file/server.cpp
OBJ = $(SRC:.cpp=.o)
CXX = g++
RM = rm -f
CPPFLAGS =   -std=c++98  -fsanitize=address -g3
NAME = webserve

all: $(NAME)



$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)