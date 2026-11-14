// anch-rest RPC tests: Thrift pet store
namespace cpp anch.test.thrift

struct Pet {
  1: required i64 id,
  2: required string name,
  3: optional list<string> tags
}

exception PetNotFound {
  1: i64 id
}

service PetStore {
  Pet getPet(1: i64 id) throws (1: PetNotFound notFound),
  Pet addPet(1: Pet pet)
}
