module.exports = function(desc) {
  var outputString = "INSERT DATA { ";

  var totalCount = 1;
  for(var i=0; i<desc.length; i++) {
    for(var j=0; j<desc[i].count; j++) {
      outputString += `$per${totalCount} `;
      outputString += desc[i].template.replace(/#/g, totalCount);
      outputString += ".\n";
      totalCount++;
    }
  }  
  outputString = outputString.slice(0, -2);
  outputString += ` 
      } WHERE { 
  `;
  totalCount = 1;
  for(var i=0; i<desc.length; i++) {
    for(var j=0; j<desc[i].count; j++) {
      outputString += `
        NEW($per${totalCount},"${desc[i].type}") .`;
      totalCount++;
    }
  }  
  outputString = outputString.slice(0, -1);
  outputString += " }";
  return outputString;
}

// var str = generateInsert([{
//   count: 150,
//   type: "person",
//   template: `<forename> "Alexis#"; <surname> "Sanchez#"`
// }]);

// console.log(str);
