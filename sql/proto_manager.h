#ifndef PROTO_MANAGER_INCLUDED
#define PROTO_MANAGER_INCLUDED

#include "google/protobuf/descriptor.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/io/tokenizer.h"

#include <map>
#include <string>

#define MAX_MESSAGE_NAME_LENGTH     1024

class Proto_manager
{
private:
  class ErrorCollectorImpl: public google::protobuf::io::ErrorCollector {
    void AddError(int line, int column, const std::string &message)
    {
      DBUG_PRINT("error", ("Tokenizer error at %d, %d: %s\n", line, column,
                           message.c_str()));
    }
    void AddWarning(int line, int column, const std::string &message)
    {
      DBUG_PRINT("error", ("Tokenizer warning at %d, %d: %s\n", line, column,
                           message.c_str()));
    }
  };
protected:
  /**
   * Used to avoid parsing the proto_definition (column comment for now)
   * every time.
   */
  std::map<std::string, google::protobuf::Descriptor*> descriptor_map;

  /**
   * Used in order to delete the pool associated with a column when that
   * column is dropped, altered or even the whole table goes.
   * TODO(fanton): Make this happen at DROP TABLE query.
   */
  std::map<std::string, google::protobuf::DescriptorPool*> pool_map;
  google::protobuf::DynamicMessageFactory factory;
  ErrorCollectorImpl err_collector;

public:
  static inline Proto_manager& get_singleton()
  {
    static Proto_manager proto_mgr;
    return proto_mgr;
  }

  google::protobuf::io::ErrorCollector& get_error_collector()
  {
    return err_collector;
  }

  bool proto_is_valid(String *str);
  google::protobuf::Message *construct_message(String *field_path,
                                               String *proto_def);
  bool encode(String *field_path, String *message, String *proto_def,
              String *output);
  bool decode(String *field_path, String *message, String *proto_def,
              String *output);

protected:
  google::protobuf::Descriptor *get_descriptor(String *field_path, String *str);
  const google::protobuf::Descriptor
    *create_descriptor(String *str,
                       google::protobuf::DescriptorPool *pool,
                       String *descr_name);

private:
  Proto_manager() {};
  Proto_manager(Proto_manager const& copy);
  Proto_manager& operator=(Proto_manager const& copy);

};
#endif /* PROTO_MANAGER_INCLUDED */
