# Colors
WHITE = \033[1;37m
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
RESET = \033[0m

NAME    = ircserv  # Change this to your desired executable name
CC      = c++                    # Change this if your compiler is not g++
CFLAGS  = -Wall -Wextra -Werror -std=c++98
SRC     = irc_server.cpp server.cpp client.cpp cmd.cpp Channel.cpp joinCmd.cpp utils.cpp kickCmd.cpp topicCmd.cpp # Add all source files here
OBJ     = ${SRC:.cpp=.o}

all: ${NAME}

${OBJ}: %.o: %.cpp
	@${CC} ${CFLAGS} -c $< -o $@

${NAME}: ${OBJ}
	@echo "$(YELLOW)Compiling $(NAME)...⏳$(RESET)"
	@${CC} ${CFLAGS} -o ${NAME} ${OBJ}
	@echo "$(GREEN)Compilation completed ✅$(RESET)"

clean:
	@${RM} ${OBJ}
	@echo "$(RED)Obj_files removed.$(RESET)"

fclean: clean
	@${RM} ${NAME}
	@echo "$(RED)Executable removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re
