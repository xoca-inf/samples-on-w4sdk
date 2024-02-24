var simpleExample =
{

    simpleF1 :function(str) 
    {
        console.log("call javascript function jsF1 with parameter:" + UTF8ToString(str))        
    },

    simpleF2: function(val)
    {
        return val + 1;      

    },


};

mergeInto(LibraryManager.library, simpleExample);

