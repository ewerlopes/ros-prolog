/******************************************************************************
 * Copyright (C) 2016 by Ralf Kaestner                                        *
 * ralf.kaestner@gmail.com                                                    *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the Lesser GNU General Public License as published by*
 * the Free Software Foundation; either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the               *
 * Lesser GNU General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the Lesser GNU General Public License   *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.       *
 ******************************************************************************/

#include <json/value.h>
#include <json/writer.h>

#include <prolog_common/Atom.h>
#include <prolog_common/Compound.h>
#include <prolog_common/Float.h>
#include <prolog_common/Integer.h>
#include <prolog_common/List.h>
#include <prolog_common/Number.h>
#include <prolog_common/Variable.h>

#include "prolog_serialization/JSONSerializer.h"

namespace prolog { namespace serialization {

/*****************************************************************************/
/* Constructors and Destructor                                               */
/*****************************************************************************/

JSONSerializer::JSONSerializer(OutputFormat outputFormat, const std::string&
    outputIndent) :
  outputFormat_(outputFormat),
  outputIndent_(outputIndent) {
}

JSONSerializer::JSONSerializer(const JSONSerializer& src) :
  outputFormat_(src.outputFormat_),
  outputIndent_(src.outputIndent_) {
}

JSONSerializer::~JSONSerializer() {  
}

/*****************************************************************************/
/* Accessors                                                                 */
/*****************************************************************************/

void JSONSerializer::setOutputFormat(OutputFormat outputFormat) {
  outputFormat_ = outputFormat;
}

JSONSerializer::OutputFormat JSONSerializer::getOutputFormat() const {
  return outputFormat_;
}

void JSONSerializer::setOutputIndent(const std::string& indent) {
  outputIndent_ = indent;
}

const std::string& JSONSerializer::getOutputIndent() const {
  return outputIndent_;
}

/*****************************************************************************/
/* Methods                                                                   */
/*****************************************************************************/

void JSONSerializer::serializeBindings(std::ostream& stream, const Bindings&
    bindings) const {
  serializeValue(stream, bindingsToValue(bindings));
}

void JSONSerializer::serializeTerm(std::ostream& stream, const Term& term)
    const {
  serializeValue(stream, termToValue(term));
}

void JSONSerializer::serializeValue(std::ostream& stream, const Json::Value&
    value) const {
  if (outputFormat_ == StyledOutput) {
    Json::StyledStreamWriter writer(outputIndent_);
    writer.write(stream, value);
  }
  else {
    Json::FastWriter writer;
    stream << writer.write(value);
  }
}

Json::Value JSONSerializer::atomToValue(const Atom& atom) const {
  return Json::Value(atom.getName());
}

Json::Value JSONSerializer::bindingsToValue(const Bindings& bindings) const {
  Json::Value value(Json::objectValue);
  
  for (Bindings::ConstIterator it = bindings.begin();
       it != bindings.end(); ++it)
    value[it->first] = termToValue(it->second);
  
  return value;
}

Json::Value JSONSerializer::compoundToValue(const Compound& compound) const {
  std::string functor = compound.getFunctor();
  std::list<Term> arguments = compound.getArguments();
  
  Json::Value argumentsValue(Json::arrayValue);
  
  for (std::list<Term>::const_iterator it = arguments.begin();
       it != arguments.end(); ++it)
    argumentsValue.append(termToValue(*it));
       
  Json::Value value(Json::objectValue);
  
  value["functor"] = functor;
  value["arguments"] = argumentsValue;  
  
  return value;
}

Json::Value JSONSerializer::floatToValue(const Float& number) const {
  return Json::Value(number.getValue());
}

Json::Value JSONSerializer::integerToValue(const Integer& number) const {
  return Json::Value(static_cast<Json::Value::Int64>(number.getValue()));
}

Json::Value JSONSerializer::numberToValue(const Number& number) const {
  if (number.isFloat())
    return floatToValue(number);
  else if (number.isInteger())
    return integerToValue(number);
}

Json::Value JSONSerializer::listToValue(const List& list) const {
  std::list<Term> elements = list.getElements();
  
  Json::Value value(Json::arrayValue);
  
  for (std::list<Term>::const_iterator it = elements.begin();
       it != elements.end(); ++it)
    value.append(termToValue(*it));
       
  return value;
}

Json::Value JSONSerializer::termToValue(const Term& term) const {
  if (term.isAtom())
    return atomToValue(term);
  else if (term.isCompound())
    return compoundToValue(term);
  else if (term.isList())
    return listToValue(term);
  else if (term.isNumber())
    return numberToValue(term);
  else if (term.isVariable())
    return variableToValue(term);
}

Json::Value JSONSerializer::variableToValue(const Variable& variable) const {
  return Json::Value(variable.getName());
}

}}
