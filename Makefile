NAME = webserv

CXX = c++

CXXFLAGS = -std=c++98  -g3 #-fsanitize=address #-Wall -Wextra -Werror

SRC  = srcs/main.cpp srcs/parsing/request/request.cpp \
	srcs/_Create_Server.cpp srcs/methods/_Post/post.cpp \
	srcs/response/response.cpp srcs/response/error_response.cpp srcs/parsing/config/config_parse.cpp\
	srcs/methods/_Post/mime_types.cpp

INC = -Iincludes

CONFIG_FILE = conf/config.conf

OBJ = $(SRC:.cpp=.o)

# .SILENT:

all: $(NAME)
	@clear
	@./$(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo -e "\033[0;32m[webserv] Compiled\033[0m"

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@
	@echo -e "\033[0;33m[webserv] Compiling \033[0m" $<

clean:
	@rm -f $(OBJ)
	@echo -e "\033[0;33mCleaned \033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo -e "\033[0;31mFcleaned \033[0m"

re: fclean all clean

.PHONY: all clean fclean re

r: $(NAME)
	@clear
	@./$(NAME) $(CONFIG_FILE)

.SECONDARY: $(OBJ)