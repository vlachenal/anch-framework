// anch-rest documentation examples: Thrift pet store
namespace cpp pets

struct Pet {
  1: required i64 id,
  2: required string name
}

exception PetNotFound {
  1: i64 id
}

service PetStore {
  Pet getPet(1: i64 id) throws (1: PetNotFound notFound)
}
