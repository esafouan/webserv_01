SRC =  multiplexer.cpp request.cpp pars.cpp socket.cpp
OBJ = $(SRC:.cpp=.o)
CXX = c++
RM = rm -f
CPPFLAGS =   -std=c++98 -fsanitize=address
NAME = webserve

all: $(NAME)



$(NAME): $(OBJ)
	$(CXX) $(CPPFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean $(NAME)