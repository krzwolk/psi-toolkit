#include "tests.hpp"

#include "layer_tag_collection.hpp"
#include "layer_tag_manager.hpp"

BOOST_AUTO_TEST_SUITE( layer_tags )

BOOST_AUTO_TEST_CASE( tags_simple ) {
    LayerTagManager layer_tag_manager;

    LayerTagCollection tag_foo = layer_tag_manager.createSingletonTagCollection("foo");
    LayerTagCollection tag_bar = layer_tag_manager.createSingletonTagCollection("bar");

    BOOST_CHECK(tag_foo != tag_bar);

    LayerTagCollection tags_empty = createIntersection(tag_foo, tag_bar);

    BOOST_CHECK(tags_empty.isEmpty());

    LayerTagCollection tags_foo_bar = createUnion(tag_foo, tag_bar);

    BOOST_CHECK(!tags_foo_bar.isEmpty());
    BOOST_CHECK(tags_foo_bar.isNonempty());

    BOOST_CHECK(tag_foo < tags_foo_bar);
    BOOST_CHECK(tag_bar < tags_foo_bar);

    std::list<std::string> tag_list = layer_tag_manager.getTagNames(tags_foo_bar);
    BOOST_CHECK_EQUAL(tag_list.size(), 2U);
    BOOST_CHECK_EQUAL(tag_list.front(), "bar");
    BOOST_CHECK_EQUAL(tag_list.back(), "foo");
}

BOOST_AUTO_TEST_CASE( tags_names_list ) {
    LayerTagManager layer_tag_manager;

    std::list<std::string> namesList;
    namesList.push_back("foo");
    namesList.push_back("bar");

    LayerTagCollection tags = layer_tag_manager.createTagCollectionFromList(namesList);

    BOOST_CHECK(!tags.isEmpty());
    BOOST_CHECK(tags.isNonempty());

    std::list<std::string> tagList = layer_tag_manager.getTagNames(tags);
    BOOST_CHECK_EQUAL(tagList.size(), 2U);
    BOOST_CHECK_EQUAL(tagList.front(), "bar");
    BOOST_CHECK_EQUAL(tagList.back(), "foo");
}

BOOST_AUTO_TEST_CASE( tags_names_vector ) {
    LayerTagManager layer_tag_manager;

    std::vector<std::string> namesVector;
    namesVector.push_back("foo");
    namesVector.push_back("bar");

    LayerTagCollection tags = layer_tag_manager.createTagCollectionFromVector(namesVector);

    BOOST_CHECK(!tags.isEmpty());
    BOOST_CHECK(tags.isNonempty());

    std::list<std::string> tagList = layer_tag_manager.getTagNames(tags);
    BOOST_CHECK_EQUAL(tagList.size(), 2U);
    BOOST_CHECK_EQUAL(tagList.front(), "bar");
    BOOST_CHECK_EQUAL(tagList.back(), "foo");
}

BOOST_AUTO_TEST_CASE( tags_masks ) {
    LayerTagManager layer_tag_manager;

    std::list<std::string> listFoo;
    listFoo.push_back("foo");
    LayerTagCollection tagFoo = layer_tag_manager.createSingletonTagCollection("foo");

    std::list<std::string> listBar;
    listBar.push_back("bar");
    LayerTagCollection tagBar = layer_tag_manager.createSingletonTagCollection("bar");

    LayerTagMask maskFoo = layer_tag_manager.getMask(tagFoo);
    LayerTagMask maskFooList = layer_tag_manager.getMask(listFoo);
    LayerTagMask maskBar = layer_tag_manager.getMask(tagBar);
    LayerTagMask maskFooBar = createUnion(maskFoo, maskBar);
    LayerTagMask maskNone = createIntersection(maskFoo, maskBar);
    LayerTagMask maskAny = layer_tag_manager.anyTag();
    LayerTagMask maskPlane = layer_tag_manager.planeTags();

    BOOST_CHECK(layer_tag_manager.match(maskFoo, "foo"));
    BOOST_CHECK(!layer_tag_manager.match(maskFoo, "bar"));
    BOOST_CHECK(!layer_tag_manager.match(maskFoo, "boo"));
    BOOST_CHECK(!layer_tag_manager.match(maskFoo, "!foo"));

    BOOST_CHECK(layer_tag_manager.match(maskFooList, "foo"));
    BOOST_CHECK(!layer_tag_manager.match(maskFooList, "bar"));
    BOOST_CHECK(!layer_tag_manager.match(maskFooList, "boo"));
    BOOST_CHECK(!layer_tag_manager.match(maskFooList, "!foo"));

    BOOST_CHECK(!layer_tag_manager.match(maskBar, "foo"));
    BOOST_CHECK(layer_tag_manager.match(maskBar, "bar"));
    BOOST_CHECK(!layer_tag_manager.match(maskBar, "boo"));
    BOOST_CHECK(!layer_tag_manager.match(maskBar, "!foo"));

    BOOST_CHECK(layer_tag_manager.match(maskFooBar, "foo"));
    BOOST_CHECK(layer_tag_manager.match(maskFooBar, "bar"));
    BOOST_CHECK(!layer_tag_manager.match(maskFooBar, "boo"));
    BOOST_CHECK(!layer_tag_manager.match(maskFooBar, "!foo"));

    BOOST_CHECK(maskNone.isNone());
    BOOST_CHECK(!layer_tag_manager.match(maskNone, "foo"));
    BOOST_CHECK(!layer_tag_manager.match(maskNone, "bar"));
    BOOST_CHECK(!layer_tag_manager.match(maskNone, "boo"));
    BOOST_CHECK(!layer_tag_manager.match(maskNone, "!foo"));

    BOOST_CHECK(maskAny.isAny());
    BOOST_CHECK(layer_tag_manager.match(maskAny, "foo"));
    BOOST_CHECK(layer_tag_manager.match(maskAny, "bar"));
    BOOST_CHECK(layer_tag_manager.match(maskAny, "boo"));
    BOOST_CHECK(layer_tag_manager.match(maskAny, "!foo"));

    BOOST_CHECK(maskPlane.isPlane());
    BOOST_CHECK(!layer_tag_manager.match(maskPlane, "foo"));
    BOOST_CHECK(!layer_tag_manager.match(maskPlane, "bar"));
    BOOST_CHECK(!layer_tag_manager.match(maskPlane, "boo"));
    BOOST_CHECK(layer_tag_manager.match(maskPlane, "!foo"));
}

BOOST_AUTO_TEST_SUITE_END()
