"""             # Handle where operation with the presence of "it" in the right operator
            if node["type"] == "WHERE" and self.is_it_present_in_opt(node):
                # Evaluate left argument
                where_left = self.eval_node(node["arg"][0])

                # Find out which kind of opearator is used on the right
                oper_kind = self.operation_handlers.get(node["arg"][1]["type"])

                match len(node["arg"][1]["arg"]):
                    case 1:
                        if self.is_list(where_left):
                            result = ListType(None)
                            for element in where_left:
                                op_result = oper_kind(element)
                                result.append(self.handle_where(element, op_result))
                            return result
                        else:
                            return self.handle_where(where_left, operation_handler(argument))
                    case 2:
                        # Evaluate the right argument of the right operator
                        oper_right = self.eval_node(node["arg"][1]["arg"][1])
                        
                        if self.is_list(where_left):
                            result = ListType(None)
                            for element in where_left:
                                op_result = oper_kind(element, oper_right)
                                result.append(self.handle_where(element, op_result))
                            return result
                        else:
                            return self.handle_where(where_left, operation_handler(where_left, oper_kind))
                    case 3:
                        mid_param = self.eval_node(node["arg"][1]["arg"][1])
                        right_param = self.eval_node(node["arg"][1]["arg"][2])

                        if self.is_list(where_left):
                            result = ListType(None)
                            for index, element in enumerate(where_left):
                                if self.is_list(right_param) :
                                    if len(where_left) == len(right_param):
                                        op_result = oper_kind(element, mid_param, right_param[index])
                                    else:
                                        return NullType(None)
                                else:
                                    op_result = oper_kind(element, mid_param, right_param)
                                result.append(self.handle_where(element, op_result))
                            return result
                        else:
                            return self.handle_where(where_left, operation_handler(where_left, oper_kind)) """
